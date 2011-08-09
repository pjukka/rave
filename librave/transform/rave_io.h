/* --------------------------------------------------------------------
Copyright (C) 2009, 2011 Swedish Meteorological and Hydrological Institute, SMHI,

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
 * Functions for performing rave related IO operations, mostly ODIM-formatted HDF5 files.
 * This object does NOT support \ref #RAVE_OBJECT_CLONE.
 *
 * @file
 * @author Anders Henja (Swedish Meteorological and Hydrological Institute, SMHI)
 * @date 2009-11-12
 */
#ifndef RAVE_IO_H
#define RAVE_IO_H

#include "rave_object.h"
#include "rave_types.h"

/**
 * The /Conventions version in a ODIM HDF5 file.
 */
typedef enum RaveIO_ODIM_Version {
  RaveIO_ODIM_Version_UNDEFINED = -1, /**< Undefined */
  RaveIO_ODIM_Version_2_0 = 0         /**< Currently, the only supported ODIM version (and default) */
} RaveIO_ODIM_Version;

/**
 * The /what/version in a ODIM HDF5 file
 */
typedef enum RaveIO_ODIM_H5rad_Version {
  RaveIO_ODIM_H5rad_Version_UNDEFINED = -1, /**< undefined */
  RaveIO_ODIM_H5rad_Version_2_0 = 0   /**< Currently, the only supported ODIM version (and default) */
} RaveIO_ODIM_H5rad_Version;

/**
 * Defines a Rave IO instance
 */
typedef struct _RaveIO_t RaveIO_t;

/**
 * Type definition to use when creating a rave object.
 */
extern RaveCoreObjectType RaveIO_TYPE;

/**
 * Closes the HDF5 file but will keep the RaveIO instance.
 * @param[in] raveio - the rave IO instance
 */
void RaveIO_close(RaveIO_t* raveio);

/**
 * Opens a supported HDF5 file and loads it into the RaveIO instance.
 * Same as:
 * RaveIO_t* instance = RAVE_OBJECT_NEW(&RaveIO_TYPE);
 * RaveIO_setFilename(instance, filename);
 * RaveIO_load(instance);
 *
 * @param[in] filename - the HDF5 file to open
 * @returns The raveio instance on success, otherwise NULL.
 */
RaveIO_t* RaveIO_open(const char* filename);

/**
 * Loads the HDF5 file into the raveio instance.
 * @param[in] raveio - self
 * @returns the opened object
 */
int RaveIO_load(RaveIO_t* raveio);

/**
 * Saves a rave object as specified according to ODIM HDF5 format specification.
 * @param[in] raveio - self
 * @param[in] filename - the filename to save with. May be NULL, same as calling RaveIO_setFilename followed by RaveIO_save(..., NULL)
 * @returns 1 on success, otherwise 0
 */
int RaveIO_save(RaveIO_t* raveio, const char* filename);

/**
 * Sets the object to be saved.
 * @param[in] raveio - self
 * @param[in] object - the object to be saved
 */
void RaveIO_setObject(RaveIO_t* raveio, RaveCoreObject* object);

/**
 * Returns the loaded object/object to be saved.
 * @param[in] raveio - self
 * @returns the object
 */
RaveCoreObject* RaveIO_getObject(RaveIO_t* raveio);

/**
 * Sets the filename that should be used when saving the object.
 * @param[in] raveio - self
 * @param[in] filename - the filename that should be used when saving.
 * @returns 1 on success, otherwise 0
 */
int RaveIO_setFilename(RaveIO_t* raveio, const char* filename);

/**
 * Returns the current filename.
 * @param[in] raveio - self
 * @returns the current filename
 */
const char* RaveIO_getFilename(RaveIO_t* raveio);

/**
 * Returns the object type for the currently opened file. Requires that
 * a RaveCoreObject has been set.
 * @param[in] raveio - the Rave IO instance
 * @returns the object type or Rave_ObjectType_UNDEFINED on error.
 */
Rave_ObjectType RaveIO_getObjectType(RaveIO_t* raveio);

/**
 * Sets the ODIM version to use when saving the file. Currently, the only
 * supported version is 2.0.
 * @param[in] raveio - self
 * @param[in] version - the version to be used
 * @returns 1 if the specified version is supported, otherwise 0.
 */
int RaveIO_setOdimVersion(RaveIO_t* raveio, RaveIO_ODIM_Version version);

/**
 * Returns the ODIM version.
 * @param[in] raveio - self
 * @returns the ODIM version
 */
RaveIO_ODIM_Version RaveIO_getOdimVersion(RaveIO_t* raveio);

/**
 * Sets the ODIM h5rad version to use when saving the file. Currently, the only
 * supported version is 2.0.
 * @param[in] raveio - self
 * @param[in] version - the version to be used
 * @returns 1 if the specified version is supported, otherwise 0.
 */
int RaveIO_setH5radVersion(RaveIO_t* raveio, RaveIO_ODIM_H5rad_Version version);

/**
 * Returns the h5rad version.
 * @param[in] raveio - self
 * @returns the h5rad version
 */
RaveIO_ODIM_H5rad_Version RaveIO_getH5radVersion(RaveIO_t* raveio);

/**
 * Sets the compression level.
 * @param[in] raveio - self
 * @param[in] lvl - the compression level (0..9)
 */
void RaveIO_setCompressionLevel(RaveIO_t* raveio, int lvl);

/**
 * Returns the compression level
 * @param[in] raveio- self
 * @returns the compression level
 */
int RaveIO_getCompressionLevel(RaveIO_t* raveio);

/**
 * Sets the user block.
 * @param[in] raveio - self
 * @param[in] userblock - the user block
 */
void RaveIO_setUserBlock(RaveIO_t* raveio, unsigned long long userblock);

/**
 * Returns the user block.
 * @param[in] raveio - self
 * @returns the user block
 */
unsigned long long RaveIO_getUserBlock(RaveIO_t* raveio);

/**
 * Sets the sizes
 * @param[in] raveio - self
 * @param[in] sz - same as sizes.sizeof_size
 * @param[in] addr - same as sizes.sizeof_addr
 */
void RaveIO_setSizes(RaveIO_t* raveio, size_t sz, size_t addr);

/**
 * Returns the sizes
 * @param[in] raveio - self
 * @param[in] sz - same as sizes.sizeof_size
 * @param[in] addr - same as sizes.sizeof_addr
 */
void RaveIO_getSizes(RaveIO_t* raveio, size_t* sz, size_t* addr);

/**
 * Sets the symk
 * @param[in] raveio - self
 * @param[in] ik - same as sym_k.ik
 * @param[in] lk - same as sym_k.lk
 */
void RaveIO_setSymk(RaveIO_t* raveio, int ik, int lk);

/**
 * Returns the symk
 * @param[in] raveio - self
 * @param[in] ik - same as sym_k.ik
 * @param[in] lk - same as sym_k.lk
 */
void RaveIO_getSymk(RaveIO_t* raveio, int* ik, int* lk);

/**
 * Sets the istore_k value.
 * @param[in] raveio - self
 * @param[in] k - the istore_k value
 */
void RaveIO_setIStoreK(RaveIO_t* raveio, long k);

/**
 * Returns the istore_k value
 * @param[in] raveio - self
 * @returns the istore_k value
 */
long RaveIO_getIStoreK(RaveIO_t* raveio);

/**
 * Sets the meta block size
 * @param[in] raveio - self
 * @param[in] sz - the meta block size
 */
void RaveIO_setMetaBlockSize(RaveIO_t* raveio, long sz);

/**
 * Returns the meta block size
 * @param[in] raveio - self
 * @returns the meta block size
 */
long RaveIO_getMetaBlockSize(RaveIO_t* raveio);

#endif
