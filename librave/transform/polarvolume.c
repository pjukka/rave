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
 * Defines the functions available when working with polar volumes
 * @file
 * @author Anders Henja (Swedish Meteorological and Hydrological Institute, SMHI)
 * @date 2009-10-15
 */
#include "polarvolume.h"
#include "polarnav.h"
#include "raveobject_list.h"
#include "rave_datetime.h"
#include "rave_debug.h"
#include "rave_alloc.h"
#include <string.h>

/**
 * This is the default parameter value that should be used when working
 * with scans.
 */
#define DEFAULT_PARAMETER_NAME "DBZH"

/**
 * Represents a volume
 */
struct _PolarVolume_t {
  RAVE_OBJECT_HEAD /** Always on top */
  Projection_t* projection; /**< projection for this volume */
  PolarNavigator_t* navigator; /**< a polar navigator */
  RaveObjectList_t* scans;  /**< the list of scans */
  RaveDateTime_t* datetime; /**< the date / time */
  char* source;             /**< the source string */
  char* paramname;          /**< the default parameter */
};

/*@{ Private functions */
/**
 * Constructor
 */
static int PolarVolume_constructor(RaveCoreObject* obj)
{
  PolarVolume_t* this = (PolarVolume_t*)obj;
  this->projection = NULL;
  this->navigator = NULL;
  this->scans = NULL;
  this->datetime = NULL;
  this->source = NULL;
  this->paramname = NULL;

  this->datetime = RAVE_OBJECT_NEW(&RaveDateTime_TYPE);

  // Always initialize to default projection for lon/lat calculations
  this->projection = RAVE_OBJECT_NEW(&Projection_TYPE);
  if (this->projection != NULL) {
    if(!Projection_init(this->projection, "lonlat", "lonlat", "+proj=latlong +ellps=WGS84 +datum=WGS84")) {
      goto error;
    }
  }
  this->navigator = RAVE_OBJECT_NEW(&PolarNavigator_TYPE);
  this->scans = RAVE_OBJECT_NEW(&RaveObjectList_TYPE);

  if (this->datetime == NULL || this->projection == NULL ||
      this->scans == NULL || this->navigator == NULL) {
    goto error;
  }

  if (!PolarVolume_setDefaultParameter(this, DEFAULT_PARAMETER_NAME)) {
    goto error;
  }

  return 1;
error:
  RAVE_OBJECT_RELEASE(this->datetime);
  RAVE_OBJECT_RELEASE(this->projection);
  RAVE_OBJECT_RELEASE(this->navigator);
  RAVE_OBJECT_RELEASE(this->scans);
  RAVE_FREE(this->source);
  RAVE_FREE(this->paramname);
  return 0;
}

/**
 * Copy constructor
 */
static int PolarVolume_copyconstructor(RaveCoreObject* obj, RaveCoreObject* srcobj)
{
  PolarVolume_t* this = (PolarVolume_t*)obj;
  PolarVolume_t* src = (PolarVolume_t*)srcobj;

  this->projection = RAVE_OBJECT_CLONE(src->projection);
  this->navigator = RAVE_OBJECT_CLONE(src->navigator);
  this->scans = RAVE_OBJECT_CLONE(src->scans); // the list only contains scans and they are cloneable
  this->datetime = RAVE_OBJECT_CLONE(src->datetime);
  this->source = NULL;
  this->paramname = NULL;

  if (this->datetime == NULL || this->projection == NULL ||
      this->scans == NULL || this->navigator == NULL) {
    goto error;
  }

  if (!PolarVolume_setSource(this, src->source)) {
    goto error;
  }
  if (!PolarVolume_setDefaultParameter(this, src->paramname)) {
    goto error;
  }

  return 1;
error:
  RAVE_FREE(this->source);
  RAVE_OBJECT_RELEASE(this->datetime);
  RAVE_OBJECT_RELEASE(this->projection);
  RAVE_OBJECT_RELEASE(this->navigator);
  RAVE_OBJECT_RELEASE(this->scans);
  RAVE_FREE(this->source);
  RAVE_FREE(this->paramname);
  return 0;
}

/**
 * Destructor
 */
static void PolarVolume_destructor(RaveCoreObject* obj)
{
  PolarVolume_t* volume = (PolarVolume_t*)obj;
  RAVE_OBJECT_RELEASE(volume->datetime);
  RAVE_OBJECT_RELEASE(volume->projection);
  RAVE_OBJECT_RELEASE(volume->navigator);
  RAVE_OBJECT_RELEASE(volume->scans);
  RAVE_FREE(volume->source);
  RAVE_FREE(volume->paramname);
}

/**
 * Returns the elevation angle for the specified scan index.
 * @param
 */
static double PolarVolumeInternal_getElangle(PolarVolume_t* pvol, int index)
{
  PolarScan_t* scan = NULL;
  double elangle = 0.0L;
  scan = (PolarScan_t*)RaveObjectList_get(pvol->scans, index);
  if (scan != NULL) {
    elangle = PolarScan_getElangle(scan);
  } else {
    RAVE_CRITICAL1("Could not fetch scan for index = %d\n", index);
  }

  RAVE_OBJECT_RELEASE(scan);
  return elangle;
}

/**
 * Used to sort the scans by elevation in ascending order
 * @param[in] a - scan a (will be casted to *(PolarScan_t**))
 * @param[in] b - scan b (will be casted to *(PolarScan_t**))
 * @return -1 if a.elangle < b.elangle, 1 if a.elangle > b.elangle and 0 otherwise
 */
static int ascendingElevationSort(const void* a, const void* b)
{
  PolarScan_t* scanA = *(PolarScan_t**)a;
  PolarScan_t* scanB = *(PolarScan_t**)b;
  double angleA = PolarScan_getElangle(scanA);
  double angleB = PolarScan_getElangle(scanB);
  if (angleA < angleB) {
    return -1;
  } else if (angleA > angleB) {
    return 1;
  }
  return 0;
}

/**
 * Used to sort the scans by elevation in descending order
 * @param[in] a - scan a (will be casted to *(PolarScan_t**))
 * @param[in] b - scan b (will be casted to *(PolarScan_t**))
 * @return -1 if a.elangle > b.elangle, 1 if a.elangle < b.elangle and 0 otherwise
 */
static int descendingElevationSort(const void* a, const void* b)
{
  PolarScan_t* scanA = *(PolarScan_t**)a;
  PolarScan_t* scanB = *(PolarScan_t**)b;
  double angleA = PolarScan_getElangle(scanA);
  double angleB = PolarScan_getElangle(scanB);
  if (angleA > angleB) {
    return -1;
  } else if (angleA < angleB) {
    return 1;
  }
  return 0;
}

/*@} End of Private functions */

/*@{ Interface functions */
int PolarVolume_setTime(PolarVolume_t* pvol, const char* value)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return RaveDateTime_setTime(pvol->datetime, value);
}

const char* PolarVolume_getTime(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return RaveDateTime_getTime(pvol->datetime);
}

int PolarVolume_setDate(PolarVolume_t* pvol, const char* value)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return RaveDateTime_setDate(pvol->datetime, value);
}

const char* PolarVolume_getDate(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return RaveDateTime_getDate(pvol->datetime);
}

int PolarVolume_setSource(PolarVolume_t* pvol, const char* value)
{
  char* tmp = NULL;
  int result = 0;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  if (value != NULL) {
    tmp = RAVE_STRDUP(value);
    if (tmp != NULL) {
      RAVE_FREE(pvol->source);
      pvol->source = tmp;
      tmp = NULL;
      result = 1;
    }
  } else {
    RAVE_FREE(pvol->source);
    result = 1;
  }
  return result;
}

const char* PolarVolume_getSource(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return (const char*)pvol->source;
}

void PolarVolume_setLongitude(PolarVolume_t* pvol, double lon)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  PolarNavigator_setLon0(pvol->navigator, lon);
}

double PolarVolume_getLongitude(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return PolarNavigator_getLon0(pvol->navigator);
}

void PolarVolume_setLatitude(PolarVolume_t* pvol, double lat)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  PolarNavigator_setLat0(pvol->navigator, lat);
}

double PolarVolume_getLatitude(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return PolarNavigator_getLat0(pvol->navigator);
}

void PolarVolume_setHeight(PolarVolume_t* pvol, double height)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  PolarNavigator_setAlt0(pvol->navigator, height);
}

double PolarVolume_getHeight(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return PolarNavigator_getAlt0(pvol->navigator);
}

void PolarVolume_setProjection(PolarVolume_t* pvol, Projection_t* projection)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  RAVE_OBJECT_RELEASE(pvol->projection);
  pvol->projection = NULL;
  if (projection != NULL) {
    int index = 0;
    int nrScans = RaveObjectList_size(pvol->scans);
    pvol->projection = RAVE_OBJECT_COPY(projection);
    for (index = 0; index < nrScans; index++) {
      PolarScan_t* scan = (PolarScan_t*)RaveObjectList_get(pvol->scans, index);
      PolarScan_setProjection(scan, projection);
      RAVE_OBJECT_RELEASE(scan);
    }
  }
}

Projection_t* PolarVolume_getProjection(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  if (pvol->projection != NULL) {
    return RAVE_OBJECT_COPY(pvol->projection);
  }
  return NULL;
}

int PolarVolume_addScan(PolarVolume_t* pvol, PolarScan_t* scan)
{
  int result = 0;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  RAVE_ASSERT((scan != NULL), "scan was NULL");
  if (RaveObjectList_add(pvol->scans, (RaveCoreObject*)scan)) {
    PolarScan_setNavigator(scan, pvol->navigator);
    PolarScan_setProjection(scan, pvol->projection);
    PolarScan_setDefaultParameter(scan, pvol->paramname);
    result = 1;
  }
  return result;
}

PolarScan_t* PolarVolume_getScan(PolarVolume_t* pvol, int index)
{
//  PolarScan_t* scan = NULL;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return (PolarScan_t*)RaveObjectList_get(pvol->scans, index);
}

int PolarVolume_getNumberOfScans(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return RaveObjectList_size(pvol->scans);
}

PolarScan_t* PolarVolume_getScanClosestToElevation(PolarVolume_t* pvol, double e, int inside)
{
  double se = 0.0L, eld = 0.0L;
  int ei = 0;
  int i = 0;
  int nrScans = 0;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");

  nrScans = RaveObjectList_size(pvol->scans);

  if (inside) {
    if ((e < PolarVolumeInternal_getElangle(pvol, 0)) ||
        (e > PolarVolumeInternal_getElangle(pvol, nrScans-1))) {
      return NULL;
    }
  }

  se = PolarVolumeInternal_getElangle(pvol, 0);
  ei = 0;
  eld = fabs(e - se);
  for (i = 1; i < nrScans; i++) {
    double elev = PolarVolumeInternal_getElangle(pvol, i);
    double elevd = fabs(e - elev);
    if (elevd < eld) {
      se = elev;
      eld = elevd;
      ei = i;
    } else {
      break;
    }
  }
  return (PolarScan_t*)RaveObjectList_get(pvol->scans, ei);
}

RaveValueType PolarVolume_getNearest(PolarVolume_t* pvol, double lon, double lat, double height, int insidee, double* v)
{
  double d = 0.0L, a = 0.0L, r = 0.0L, e = 0.0L;
  RaveValueType result = RaveValueType_NODATA;
  PolarScan_t* scan = NULL;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  RAVE_ASSERT((v != NULL), "v was NULL");
  *v = 0.0;

  PolarNavigator_llToDa(pvol->navigator, lat, lon, &d, &a);
  PolarNavigator_dhToRe(pvol->navigator, d, height, &r, &e);

  // Find relevant elevation
  scan = PolarVolume_getScanClosestToElevation(pvol, e, insidee);
  if (scan != NULL) {
    //@todo: Eventually use the actual elevation and calculate proper range instead.
    // Now we have the elevation angle, fetch value by providing azimuth and range.
    result = PolarScan_getValueAtAzimuthAndRange(scan, a, r, v);
  }

  RAVE_OBJECT_RELEASE(scan);

  return result;
}

int PolarVolume_setDefaultParameter(PolarVolume_t* pvol, const char* quantity)
{
  int result = 0;
  char* tmp = NULL;
  RAVE_ASSERT((pvol != NULL), "pvol == NULL");
  RAVE_ASSERT((quantity != NULL), "quantity == NULL");
  tmp = RAVE_STRDUP(quantity);
  if (tmp != NULL) {
    int i = 0;
    int nlen = RaveObjectList_size(pvol->scans);
    result = 1; /* Asume everything is ok and let the scans default parameter decide the result */
    RAVE_FREE(pvol->paramname);
    pvol->paramname = tmp;
    for (i = 0; result == 1 && i < nlen; i++) {
      PolarScan_t* scan = (PolarScan_t*)RaveObjectList_get(pvol->scans, i);
      if (scan != NULL) {
        result = PolarScan_setDefaultParameter(scan, quantity);
      }
      RAVE_OBJECT_RELEASE(scan);
    }
  }
  return result;
}

const char* PolarVolume_getDefaultParameter(PolarVolume_t* pvol)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  return (const char*)pvol->paramname;
}

void PolarVolume_sortByElevations(PolarVolume_t* pvol, int ascending)
{
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");

  if (ascending == 1) {
    RaveObjectList_sort(pvol->scans, ascendingElevationSort);
  } else {
    RaveObjectList_sort(pvol->scans, descendingElevationSort);
  }
}

int PolarVolume_isAscendingScans(PolarVolume_t* pvol)
{
  int result = 1;
  int index = 0;
  double lastelev = 0.0L;
  int nrScans = 0;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  nrScans = RaveObjectList_size(pvol->scans);
  if (nrScans > 0) {
    lastelev = PolarVolumeInternal_getElangle(pvol, 0);
    for (index = 1; result == 1 && index < nrScans; index++) {
      double nextelev = PolarVolumeInternal_getElangle(pvol, index);
      if (nextelev < lastelev) {
        result = 0;
      }
      lastelev = nextelev;
    }
  }
  return result;
}

int PolarVolume_isTransformable(PolarVolume_t* pvol)
{
  int result = 0;
  RAVE_ASSERT((pvol != NULL), "pvol was NULL");
  // Verify that the volume at least got one scan and that the scans
  // are sorted in ascending order.
  if (RaveObjectList_size(pvol->scans) > 0 && PolarVolume_isAscendingScans(pvol)) {
    result = 1;
  }
  return result;
}

/*@} End of Interface functions */
RaveCoreObjectType PolarVolume_TYPE = {
    "PolarVolume",
    sizeof(PolarVolume_t),
    PolarVolume_constructor,
    PolarVolume_destructor,
    PolarVolume_copyconstructor
};
