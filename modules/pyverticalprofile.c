/* --------------------------------------------------------------------
Copyright (C) 2009 Swedish Meteorological and Hydrological Institute, SMHI,

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
 * Python version of the VerticalProfile API.
 * @file
 * @author Anders Henja (Swedish Meteorological and Hydrological Institute, SMHI)
 * @date 2012-08-24
 */
#include "Python.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define PYVERTICALPROFILE_MODULE   /**< to get correct part of pyverticalprofile,h */
#include "pyverticalprofile.h"

#include <arrayobject.h>
#include "pyrave_debug.h"
#include "pyravefield.h"
#include "rave_alloc.h"
#include "raveutil.h"
#include "rave.h"

/**
 * Debug this module
 */
PYRAVE_DEBUG_MODULE("_verticalprofile");

/**
 * Sets a python exception and goto tag
 */
#define raiseException_gotoTag(tag, type, msg) \
{PyErr_SetString(type, msg); goto tag;}

/**
 * Sets a python exception and return NULL
 */
#define raiseException_returnNULL(type, msg) \
{PyErr_SetString(type, msg); return NULL;}

/**
 * Error object for reporting errors to the python interpreeter
 */
static PyObject *ErrorObject;

/// --------------------------------------------------------------------
/// Vertical Profile
/// --------------------------------------------------------------------
/*@{ Vertical Profile */
/**
 * Returns the native VerticalProfile_t instance.
 * @param[in] pyobj - the python vertical profile instance
 * @returns the native vertical profile instance.
 */
static VerticalProfile_t*
PyVerticalProfile_GetNative(PyVerticalProfile* pyobj)
{
  RAVE_ASSERT((pyobj != NULL), "pyobj == NULL");
  return RAVE_OBJECT_COPY(pyobj->vp);
}

/**
 * Creates a python vertical profile from a native vertical profile or will create an
 * initial native VerticalProfile if p is NULL.
 * @param[in] p - the native vertical profile (or NULL)
 * @returns the python vertical profile
 */
static PyVerticalProfile* PyVerticalProfile_New(VerticalProfile_t* p)
{
  PyVerticalProfile* result = NULL;
  VerticalProfile_t* cp = NULL;

  if (p == NULL) {
    cp = RAVE_OBJECT_NEW(&VerticalProfile_TYPE);
    if (cp == NULL) {
      RAVE_CRITICAL0("Failed to allocate memory for vertical profile.");
      raiseException_returnNULL(PyExc_MemoryError, "Failed to allocate memory for vertical profile.");
    }
  } else {
    cp = RAVE_OBJECT_COPY(p);
    result = RAVE_OBJECT_GETBINDING(p); // If p already have a binding, then this should only be increfed.
    if (result != NULL) {
      Py_INCREF(result);
    }
  }

  if (result == NULL) {
    result = PyObject_NEW(PyVerticalProfile, &PyVerticalProfile_Type);
    if (result != NULL) {
      PYRAVE_DEBUG_OBJECT_CREATED;
      result->vp = RAVE_OBJECT_COPY(cp);
      RAVE_OBJECT_BIND(result->vp, result);
    } else {
      RAVE_CRITICAL0("Failed to create PyVerticalProfile instance");
      raiseException_gotoTag(done, PyExc_MemoryError, "Failed to allocate memory for vertical profile.");
    }
  }
done:
  RAVE_OBJECT_RELEASE(cp);
  return result;
}

/**
 * Deallocates the vertical profile
 * @param[in] obj the object to deallocate.
 */
static void _pyverticalprofile_dealloc(PyVerticalProfile* obj)
{
  if (obj == NULL) {
    return;
  }
  PYRAVE_DEBUG_OBJECT_DESTROYED;
  RAVE_OBJECT_UNBIND(obj->vp, obj);
  RAVE_OBJECT_RELEASE(obj->vp);
  PyObject_Del(obj);
}

/**
 * Creates a new instance of the vertical profile.
 * @param[in] self this instance.
 * @param[in] args arguments for creation (NOT USED).
 * @return the object on success, otherwise NULL
 */
static PyObject* _pyverticalprofile_new(PyObject* self, PyObject* args)
{
  PyVerticalProfile* result = PyVerticalProfile_New(NULL);
  return (PyObject*)result;
}

static PyObject* _pyverticalprofile_setLevels(PyVerticalProfile* self, PyObject* args)
{
  long l = 0;
  if (!PyArg_ParseTuple(args, "l", &l)) {
    return NULL;
  }
  if (!VerticalProfile_setLevels(self->vp, l)){
    raiseException_returnNULL(PyExc_AttributeError, "Failed to set level count");
  }
  Py_RETURN_NONE;
}

static PyObject* _pyverticalprofile_getLevels(PyVerticalProfile* self, PyObject* args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  return PyLong_FromLong(VerticalProfile_getLevels(self->vp));
}

/**
 * Sets the ff field (Mean horizontal wind velocity (m/s))
 * @param[in] self - this instance
 * @param[in] args - object, of type RaveFieldCore
 * @returns None
 */
static PyObject* _pyverticalprofile_setFF(PyVerticalProfile* self, PyObject* args)
{
  PyObject* inptr = NULL;
  PyRaveField* ravefield = NULL;

  if (!PyArg_ParseTuple(args, "O", &inptr)) {
    return NULL;
  }

  if (!PyRaveField_Check(inptr)) {
    raiseException_returnNULL(PyExc_TypeError,"Object must be of type RaveFieldCore");
  }

  ravefield = (PyRaveField*)inptr;

  if (!VerticalProfile_setFF(self->vp, ravefield->field)) {
    raiseException_returnNULL(PyExc_AttributeError, "Failed to set FF");
  }

  Py_RETURN_NONE;
}

/**
 * Returns the ff field (Mean horizontal wind velocity (m/s))
 * @param[in] self - this instance
 * @param[in] args - N/A
 * @returns The ff field
 */
static PyObject* _pyverticalprofile_getFF(PyVerticalProfile* self, PyObject* args)
{
  PyObject* result = NULL;
  RaveField_t* field = NULL;
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  if ((field = VerticalProfile_getFF(self->vp)) == NULL) {
    Py_RETURN_NONE;
  }
  result = (PyObject*)PyRaveField_New(field);
  RAVE_OBJECT_RELEASE(field);
  return result;
}


/**
 * All methods a polar scan can have
 */
static struct PyMethodDef _pyverticalprofile_methods[] =
{
  {"time", NULL},
  {"date", NULL},
  {"source", NULL},
  {"longitude", NULL},
  {"latitude", NULL},
  {"height", NULL},
  {"interval", NULL},
  {"minheight", NULL},
  {"maxheight", NULL},
  {"setLevels", (PyCFunction) _pyverticalprofile_setLevels, 1},
  {"getLevels", (PyCFunction) _pyverticalprofile_getLevels, 1},
  {"getFF", (PyCFunction) _pyverticalprofile_getFF, 1},
  {"setFF", (PyCFunction) _pyverticalprofile_setFF, 1},
  {NULL, NULL}
};

/**
 * Returns the specified attribute in the vertical profile
 */
static PyObject* _pyverticalprofile_getattr(PyVerticalProfile* self, char* name)
{
  PyObject* res;
  if (strcmp("time", name) == 0) {
    const char* str = VerticalProfile_getTime(self->vp);
    if (str != NULL) {
      return PyString_FromString(str);
    } else {
      Py_RETURN_NONE;
    }
  } else if (strcmp("date", name) == 0) {
    const char* str = VerticalProfile_getDate(self->vp);
    if (str != NULL) {
      return PyString_FromString(str);
    } else {
      Py_RETURN_NONE;
    }
  } else if (strcmp("source", name) == 0) {
    const char* str = VerticalProfile_getSource(self->vp);
    if (str != NULL) {
      return PyString_FromString(str);
    } else {
      Py_RETURN_NONE;
    }
  } else if (strcmp("longitude", name) == 0) {
    return PyFloat_FromDouble(VerticalProfile_getLongitude(self->vp));
  } else if (strcmp("latitude", name) == 0) {
    return PyFloat_FromDouble(VerticalProfile_getLatitude(self->vp));
  } else if (strcmp("height", name) == 0) {
    return PyFloat_FromDouble(VerticalProfile_getHeight(self->vp));
  } else if (strcmp("interval", name) == 0) {
    return PyFloat_FromDouble(VerticalProfile_getInterval(self->vp));
  } else if (strcmp("minheight", name) == 0) {
    return PyFloat_FromDouble(VerticalProfile_getMinheight(self->vp));
  } else if (strcmp("maxheight", name) == 0) {
    return PyFloat_FromDouble(VerticalProfile_getMaxheight(self->vp));
  }

  res = Py_FindMethod(_pyverticalprofile_methods, (PyObject*) self, name);
  if (res)
    return res;

  PyErr_Clear();
  PyErr_SetString(PyExc_AttributeError, name);
  return NULL;
}

/**
 * Returns the specified attribute in the vertical profile
 */
static int _pyverticalprofile_setattr(PyVerticalProfile* self, char* name, PyObject* val)
{
  int result = -1;
  if (name == NULL) {
    goto done;
  }
  if (strcmp("time", name) == 0) {
    if (PyString_Check(val)) {
      if (!VerticalProfile_setTime(self->vp, PyString_AsString(val))) {
        raiseException_gotoTag(done, PyExc_ValueError, "time must be a string (HHmmss)");
      }
    } else if (val == Py_None) {
      VerticalProfile_setTime(self->vp, NULL);
    } else {
      raiseException_gotoTag(done, PyExc_ValueError, "time must be a string (HHmmss)");
    }
  } else if (strcmp("date", name) == 0) {
    if (PyString_Check(val)) {
      if (!VerticalProfile_setDate(self->vp, PyString_AsString(val))) {
        raiseException_gotoTag(done, PyExc_ValueError, "date must be a string (YYYYMMSS)");
      }
    } else if (val == Py_None) {
      VerticalProfile_setDate(self->vp, NULL);
    } else {
      raiseException_gotoTag(done, PyExc_ValueError, "date must be a string (YYYYMMSS)");
    }
  } else if (strcmp("source", name) == 0) {
    if (PyString_Check(val)) {
      if (!VerticalProfile_setSource(self->vp, PyString_AsString(val))) {
        raiseException_gotoTag(done, PyExc_ValueError, "source must be a string");
      }
    } else if (val == Py_None) {
      VerticalProfile_setSource(self->vp, NULL);
    } else {
      raiseException_gotoTag(done, PyExc_ValueError, "source must be a string");
    }
  } else if (strcmp("longitude", name) == 0) {
    if (PyFloat_Check(val)) {
      VerticalProfile_setLongitude(self->vp, PyFloat_AsDouble(val));
    } else {
      raiseException_gotoTag(done, PyExc_TypeError, "longitude must be of type float");
    }
  } else if (strcmp("latitude", name) == 0) {
    if (PyFloat_Check(val)) {
      VerticalProfile_setLatitude(self->vp, PyFloat_AsDouble(val));
    } else {
      raiseException_gotoTag(done, PyExc_TypeError, "latitude must be of type float");
    }
  } else if (strcmp("height", name) == 0) {
    if (PyFloat_Check(val)) {
      VerticalProfile_setHeight(self->vp, PyFloat_AsDouble(val));
    } else {
      raiseException_gotoTag(done, PyExc_TypeError, "height must be of type float");
    }
  } else if (strcmp("interval", name) == 0) {
    if (PyFloat_Check(val)) {
      VerticalProfile_setInterval(self->vp, PyFloat_AsDouble(val));
    } else {
      raiseException_gotoTag(done, PyExc_TypeError, "interval must be of type float");
    }
  } else if (strcmp("minheight", name) == 0) {
    if (PyFloat_Check(val)) {
      VerticalProfile_setMinheight(self->vp, PyFloat_AsDouble(val));
    } else {
      raiseException_gotoTag(done, PyExc_TypeError, "minheight must be of type float");
    }
  } else if (strcmp("maxheight", name) == 0) {
    if (PyFloat_Check(val)) {
      VerticalProfile_setMaxheight(self->vp, PyFloat_AsDouble(val));
    } else {
      raiseException_gotoTag(done, PyExc_TypeError, "maxheight must be of type float");
    }
  } else {
    raiseException_gotoTag(done, PyExc_AttributeError, name);
  }

  result = 0;
done:
  return result;
}

static PyObject* _pyverticalprofile_isVerticalProfile(PyObject* self, PyObject* args)
{
  PyObject* inobj = NULL;
  if (!PyArg_ParseTuple(args,"O", &inobj)) {
    return NULL;
  }
  if (PyVerticalProfile_Check(inobj)) {
    return PyBool_FromLong(1);
  }
  return PyBool_FromLong(0);
}
/*@} End of Vertical Profile */

/// --------------------------------------------------------------------
/// Type definitions
/// --------------------------------------------------------------------
/*@{ Type definition */
PyTypeObject PyVerticalProfile_Type =
{
  PyObject_HEAD_INIT(NULL)0, /*ob_size*/
  "VerticalProfileCore", /*tp_name*/
  sizeof(PyVerticalProfile), /*tp_size*/
  0, /*tp_itemsize*/
  /* methods */
  (destructor)_pyverticalprofile_dealloc, /*tp_dealloc*/
  0, /*tp_print*/
  (getattrfunc)_pyverticalprofile_getattr, /*tp_getattr*/
  (setattrfunc)_pyverticalprofile_setattr, /*tp_setattr*/
  0, /*tp_compare*/
  0, /*tp_repr*/
  0, /*tp_as_number */
  0,
  0, /*tp_as_mapping */
  0 /*tp_hash*/
};
/*@} End of Type definition */

/*@{ Module setup */
static PyMethodDef functions[] = {
  {"new", (PyCFunction)_pyverticalprofile_new, 1},
  {"isVerticalProfile", (PyCFunction)_pyverticalprofile_isVerticalProfile, 1},
  {NULL,NULL} /*Sentinel*/
};

PyMODINIT_FUNC
init_verticalprofile(void)
{
  PyObject *module=NULL,*dictionary=NULL;
  static void *PyVerticalProfile_API[PyVerticalProfile_API_pointers];
  PyObject *c_api_object = NULL;
  PyVerticalProfile_Type.ob_type = &PyType_Type;

  module = Py_InitModule("_verticalprofile", functions);
  if (module == NULL) {
    return;
  }
  PyVerticalProfile_API[PyVerticalProfile_Type_NUM] = (void*)&PyVerticalProfile_Type;
  PyVerticalProfile_API[PyVerticalProfile_GetNative_NUM] = (void *)PyVerticalProfile_GetNative;
  PyVerticalProfile_API[PyVerticalProfile_New_NUM] = (void*)PyVerticalProfile_New;

  c_api_object = PyCObject_FromVoidPtr((void *)PyVerticalProfile_API, NULL);

  if (c_api_object != NULL) {
    PyModule_AddObject(module, "_C_API", c_api_object);
  }

  dictionary = PyModule_GetDict(module);
  ErrorObject = PyString_FromString("_verticalprofile.error");
  if (ErrorObject == NULL || PyDict_SetItemString(dictionary, "error", ErrorObject) != 0) {
    Py_FatalError("Can't define _verticalprofile.error");
  }

  import_array(); /*To make sure I get access to Numeric*/
  import_pyravefield();
  PYRAVE_DEBUG_INITIALIZE;
}
/*@} End of Module setup */
