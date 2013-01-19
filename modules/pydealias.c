/* --------------------------------------------------------------------
Copyright (C) 2011 Swedish Meteorological and Hydrological Institute, SMHI,

This file is part of RAVE.

RAVE is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RAVE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with RAVE.  If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------------------------------*/
/**
 * Python version of the dealiasing API
 * @file
 * @author Daniel Michelson (Swedish Meteorological and Hydrological Institute, SMHI)
 * @date 2012-11-15
 */
#include "Python.h"
#include "arrayobject.h"
#include "rave.h"
#include "rave_debug.h"
#include "dealias.h"
#include "pypolarvolume.h"
#include "pypolarscan.h"
#include "pyrave_debug.h"

/**
 * Debug this module
 */
PYRAVE_DEBUG_MODULE("_dealias");

/**
 * Sets a Python exception.
 */
#define Raise(type,msg) {PyErr_SetString(type,msg);}

/**
 * Sets a Python exception and goto tag
 */
#define raiseException_gotoTag(tag, type, msg) \
{PyErr_SetString(type, msg); goto tag;}

/**
 * Sets a Python exception and return NULL
 */
#define raiseException_returnNULL(type, msg) \
{PyErr_SetString(type, msg); return NULL;}

/**
 * Error object for reporting errors to the Python interpreter
 */
static PyObject *ErrorObject;


/**
 * Checks whether a scan is dealiased by looking up its
 * "VRAD" param/how/dealiased attribute.
 * @param[in] PolarScan_t object, hopefully containing a "VRAD" parameter
 * @returns Py_True or Py_False
 */
static PyObject* _dealiased_func(PyObject* self, PyObject* args) {
  PyObject* object = NULL;
  PyPolarScan* pyscan = NULL;

  if (!PyArg_ParseTuple(args, "O", &object)) {
    return NULL;
  }

  if (PyPolarScan_Check(object)) {
    pyscan = (PyPolarScan*)object;
  } else {
    raiseException_returnNULL(PyExc_AttributeError, "Dealiased check requires scan as input");
  }

  if (dealiased(pyscan->scan)) {
    Py_RETURN_TRUE;
  }
  Py_RETURN_FALSE;
}


/**
 * Dealiasing of polar scan polar volume
 * @param[in]
 * @returns Py_None
 */
static PyObject* _dealias_func(PyObject* self, PyObject* args)
{
  PyObject* object = NULL;
  PyPolarScan* scan = NULL;
  PyPolarVolume* volume = NULL;
  int ret = 0;

  if (!PyArg_ParseTuple(args, "O", &object)) {
    return NULL;
  }

  if (PyPolarVolume_Check(object)) {
    volume = (PyPolarVolume*)object;
  } else if (PyPolarScan_Check(object)) {
    scan = (PyPolarScan*)object;
  } else {
    raiseException_returnNULL(PyExc_AttributeError, "Dealiasing requires scan or volume as input");
  }

  if (PyPolarVolume_Check(object)) {
    ret = dealias_pvol(volume->pvol);
  } else {
    ret = dealias_scan(scan->scan);
  }

  if (ret) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}


static struct PyMethodDef _dealias_functions[] =
{
  { "dealiased", (PyCFunction) _dealiased_func, METH_VARARGS },
  { "dealias", (PyCFunction) _dealias_func, METH_VARARGS },
  { NULL, NULL }
};

/**
 * Initialize the _dealias module
 */
PyMODINIT_FUNC init_dealias(void)
{
  PyObject* m;
  m = Py_InitModule("_dealias", _dealias_functions);
  ErrorObject = PyString_FromString("_dealias.error");

  if (ErrorObject == NULL || PyDict_SetItemString(PyModule_GetDict(m),
                                                  "error", ErrorObject) != 0) {
    Py_FatalError("Can't define _dealias.error");
  }
  import_pypolarvolume();
  import_pypolarscan();
  import_array(); /*To make sure I get access to Numeric*/
  PYRAVE_DEBUG_INITIALIZE;
}

/*@} End of Module setup */