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
 * Defines the functions available when working with polar scans.
 * This object supports \ref #RAVE_OBJECT_CLONE.
 * @file
 * @author Anders Henja (Swedish Meteorological and Hydrological Institute, SMHI)
 * @date 2009-10-15
 */
#ifndef POLARSCAN_H
#define POLARSCAN_H
#include "polarnav.h"
#include "projection.h"
#include "polarscanparam.h"
#include "rave_object.h"
#include "rave_types.h"
#include "rave_list.h"
#include "raveobject_list.h"
#include "rave_field.h"

/**
 * Defines a Polar Scan
 */
typedef struct _PolarScan_t PolarScan_t;

/**
 * Type definition to use when creating a rave object.
 */
extern RaveCoreObjectType PolarScan_TYPE;

/**
 * Sets a navigator for the polar scan, this is preferrable to use
 * when this scan is included in a volume since the settings will
 * be identical for all scans included in the volume. Otherwise,
 * if the scan is managed separately, use longitude/latitude and height
 * instead.
 * @param[in] scan - the scan
 * @param[in] navigator - the polar navigator (MAY NOT BE NULL)
 */
void PolarScan_setNavigator(PolarScan_t* scan, PolarNavigator_t* navigator);

/**
 * Returns the navigator that is used for this scan.
 * @param[in] scan - the scan
 * @returns the polar navigator
 */
PolarNavigator_t* PolarScan_getNavigator(PolarScan_t* scan);

/**
 * Sets the projection to be used for this scan.
 * @param[in] scan - the scan
 * @param[in] projection - the projection (MAY NOT BE NULL)
 */
void PolarScan_setProjection(PolarScan_t* scan, Projection_t* projection);

/**
 * Returns the current projection for this scan.
 * @param[in] scan - the scan
 * @returns the projection used within this scan.
 */
Projection_t* PolarScan_getProjection(PolarScan_t* scan);

/**
 * Sets the nominal time.
 * @param[in] scan - self
 * @param[in] value - the time in the format HHmmss
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setTime(PolarScan_t* scan, const char* value);

/**
 * Returns the nominal time.
 * @param[in] scan - self
 * @returns the nominal time (or NULL if there is none)
 */
const char* PolarScan_getTime(PolarScan_t* scan);

/**
 * Sets the start time.
 * @param[in] scan - self
 * @param[in] value - the time in the format HHmmss
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setStartTime(PolarScan_t* scan, const char* value);

/**
 * Returns the start time.
 * @param[in] scan - self
 * @returns the start time (or NULL if there is none)
 */
const char* PolarScan_getStartTime(PolarScan_t* scan);

/**
 * Sets the end time.
 * @param[in] scan - self
 * @param[in] value - the time in the format HHmmss
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setEndTime(PolarScan_t* scan, const char* value);

/**
 * Returns the end time.
 * @param[in] scan - self
 * @returns the end time (or NULL if there is none)
 */
const char* PolarScan_getEndTime(PolarScan_t* scan);

/**
 * Sets the nominal date.
 * @param[in] scan - self
 * @param[in] value - the date in the format YYYYMMDD
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setDate(PolarScan_t* scan, const char* value);

/**
 * Returns the nominal date.
 * @param[in] scan - self
 * @returns the nominal time (or NULL if there is none)
 */
const char* PolarScan_getDate(PolarScan_t* scan);

/**
 * Sets the start date.
 * @param[in] scan - self
 * @param[in] value - the date in the format YYYYMMDD
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setStartDate(PolarScan_t* scan, const char* value);

/**
 * Returns the start date.
 * @param[in] scan - self
 * @returns the start date (or NULL if there is none)
 */
const char* PolarScan_getStartDate(PolarScan_t* scan);

/**
 * Sets the end date.
 * @param[in] scan - self
 * @param[in] value - the date in the format YYYYMMDD
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setEndDate(PolarScan_t* scan, const char* value);

/**
 * Returns the end date.
 * @param[in] scan - self
 * @returns the end date (or NULL if there is none)
 */
const char* PolarScan_getEndDate(PolarScan_t* scan);

/**
 * Sets the source.
 * @param[in] scan - self
 * @param[in] value - the source
 * @returns 1 on success, otherwise 0
 */
int PolarScan_setSource(PolarScan_t* scan, const char* value);

/**
 * Returns the source.
 * @param[in] scan - self
 * @returns the source or NULL if there is none
 */
const char* PolarScan_getSource(PolarScan_t* scan);

/**
 * Sets the longitude
 * @param[in] scan - self
 * @param[in] lon - the longitude
 */
void PolarScan_setLongitude(PolarScan_t* scan, double lon);

/**
 * Returns the longitude
 * @param[in] scan - self
 * @returns the longitude
 */
double PolarScan_getLongitude(PolarScan_t* scan);

/**
 * Sets the latitude
 * @param[in] scan - self
 * @param[in] lat - the latitude
 */
void PolarScan_setLatitude(PolarScan_t* scan, double lat);

/**
 * Returns the latitude
 * @param[in] scan - self
 * @returns the latitude
 */
double PolarScan_getLatitude(PolarScan_t* scan);

/**
 * Sets the height
 * @param[in] scan - self
 * @param[in] height - the height
 */
void PolarScan_setHeight(PolarScan_t* scan, double height);

/**
 * Returns the height
 * @param[in] scan - self
 * @returns the height
 */
double PolarScan_getHeight(PolarScan_t* scan);

/**
 * Returns the distance from the radar to the specified lon/lat coordinate pair.
 * @param[in] scan - self
 * @param[in] lon - the longitude
 * @param[in] lat - the latitude
 * @returns the distance in meters.
 */
double PolarScan_getDistance(PolarScan_t* scan, double lon, double lat);

/**
 * Returns the maximum distance (at ground level) that this scan will cover.
 * @param[in] scan
 * @return the maximum distance in meters
 */
double PolarScan_getMaxDistance(PolarScan_t* scan);

/**
 * Sets the elevation angle for the scan
 * @param[in] scan - self
 * @param[in] elangle - the elevation angle
 */
void PolarScan_setElangle(PolarScan_t* scan, double elangle);

/**
 * Returns the elevation angle for the scan
 * @param[in] scan - the scan
 * @return the elevation angle
 */
double PolarScan_getElangle(PolarScan_t* scan);

/**
 * Returns the number of bins
 * @param[in] scan - the scan
 * @return the number of bins
 */
long PolarScan_getNbins(PolarScan_t* scan);

/**
 * Sets the range scale for the scan
 * @param[in] scan - the scan
 * @param[in] rscale - the scale of the range bin
 */
void PolarScan_setRscale(PolarScan_t* scan, double rscale);

/**
 * Returns the range bin scale for the scan
 * @param[in] scan - the scan
 * @return the scale of the range bin
 */
double PolarScan_getRscale(PolarScan_t* scan);

/**
 * Returns the number of rays/scan
 * @param[in] scan - the scan
 * @return the number of rays
 */
long PolarScan_getNrays(PolarScan_t* scan);

/**
 * Sets the ray start for the scan
 * @param[in] scan - the scan
 * @param[in] rstart - the start position of the ray
 */
void PolarScan_setRstart(PolarScan_t* scan, double rstart);

/**
 * Returns the ray start for the scan
 * @param[in] scan - the scan
 * @return the ray start position
 */
double PolarScan_getRstart(PolarScan_t* scan);

/**
 * Returns the data type
 * @param[in] scan - the scan
 * @return the data type
 */
RaveDataType PolarScan_getDataType(PolarScan_t* scan);

/**
 * Sets the a1gate
 * @param[in] scan - the scan
 * @param[in] a1gate - a1gate
 */
void PolarScan_setA1gate(PolarScan_t* scan, long a1gate);

/**
 * Returns the a1gate
 * @param[in] scan - the scan
 * @return the a1gate
 */
long PolarScan_getA1gate(PolarScan_t* scan);

/**
 * Sets the beamwidth. Default is 1.0 * M_PI/360.0
 * @param[in] scan - the polar scan
 * @param[in] beamwidth - the beam width in radians
 */
void PolarScan_setBeamwidth(PolarScan_t* scan, double beamwidth);

/**
 * Returns the beamwidth. Default is 1.0 * M_PI/360.0.
 * @param[in] scan - the polar scan
 * @return the beam width om radians
 */
double PolarScan_getBeamwidth(PolarScan_t* scan);

/**
 * Sets the default parameter for this scan. I.e. all operations
 * that retrieves/sets values that does not contain a parameter name
 * as well will use the default parameter. Note, there is nothing
 * verifying if the parameter actually exists so if you are uncertain
 * use \ref #hasParameter first.
 * @param[in] scan - self
 * @param[in] quantity - the parameter
 * @returns 1 on success otherwise 0
 */
int PolarScan_setDefaultParameter(PolarScan_t* scan, const char* quantity);

/**
 * Returns the currently specified default parameter name.
 * @param[in] scan - self
 * @returns the default parameter name
 */
const char* PolarScan_getDefaultParameter(PolarScan_t* scan);

/**
 * Adds a parameter to the polar scan. Note, if there already exists
 * a parameter with the same quantity, that parameter will be replaced
 * by this. Also, several consistency checks will be performed to ensure
 * that dimensions and similar are the same for all parameters that
 * are added.
 * @param[in] scan - self
 * @param[in] parameter - the parameter
 * @returns 1 on success, otherwise 0
 */
int PolarScan_addParameter(PolarScan_t* scan, PolarScanParam_t* parameter);

/**
 * Removes (and returns) the parameter that is specified by the quantity.
 * Note, since the parameter returned is inc-refed, remember to release it.
 * @param[in] scan - self
 * @param[in] key - the quantity name
 * @returns NULL if nothing found or the parameter if it exists.
 */
PolarScanParam_t* PolarScan_removeParameter(PolarScan_t* scan, const char* quantity);

/**
 * Removes all parameters that are in the scan as well as dereferencing
 * the default parameter.
 * @param[in] scan - self
 * @returns 1 on success or 0 on failure (if default name can not be reset)
 */
int PolarScan_removeAllParameters(PolarScan_t* scan);

/**
 * Returns the parameter that is specified by the quantity.
 * Note, since the parameter returned is inc-refed, remember to release it.
 * @param[in] scan - self
 * @param[in] key - the quantity name
 * @returns NULL if nothing found or the parameter if it exists.
 */
PolarScanParam_t* PolarScan_getParameter(PolarScan_t* scan, const char* quantity);

/**
 * Returns all parameters belonging to this scan.
 * @param[in] scan - self
 * @returns a list of 0 or more parameters on success otherwise NULL.
 */
RaveObjectList_t* PolarScan_getParameters(PolarScan_t* scan);

/**
 * Returns if the scan contains the specified parameter or not.
 * @param[in] scan - self
 * @param[in] quantity - the quantity name
 * @returns 1 if the parameter exists, otherwise 0
 */
int PolarScan_hasParameter(PolarScan_t* scan, const char* quantity);

/**
 * Returns this scans parameter names.
 * @param[in] scan - self
 * @returns this scans contained parameters. NULL on failure. Use \ref #RaveList_freeAndDestroy to destroy
 */
RaveList_t* PolarScan_getParameterNames(PolarScan_t* scan);

/**
 * Adds a quality field to this scan.
 * @param[in] scan - self
 * @param[in] field - the field to add
 * @returns 1 on success otherwise 0
 */
int PolarScan_addQualityField(PolarScan_t* scan, RaveField_t* field);

/**
 * Returns the quality field at the specified location.
 * @param[in] scan - self
 * @param[in] index - the index
 * @returns the quality field if found, otherwise NULL
 */
RaveField_t* PolarScan_getQualityField(PolarScan_t* scan, int index);

/**
 * Returns the number of quality fields
 * @param[in] scan - self
 * @returns the number of quality fields
 */
int PolarScan_getNumberOfQualityFields(PolarScan_t* scan);

/**
 * Removes the quality field at the specified location
 * @param[in] scan - self
 * @param[in] index - the index
 */
void PolarScan_removeQualityField(PolarScan_t* scan, int index);

/**
 * Returns all quality fields belonging to this scan. The returned
 * object is only a reference so do not modify it.
 * @param[in] scan - self
 * @returns a list of 0 or more quality fields or NULL on error.
 */
RaveObjectList_t* PolarScan_getQualityFields(PolarScan_t* scan);

/**
 * Returns a quality field based on the value of how/task that should be a
 * string.
 * @param[in] scan - self
 * @param[in] value - the value of the how/task attribute
 * @return the field if found otherwise NULL
 */
RaveField_t* PolarScan_getQualityFieldByHowTask(PolarScan_t* scan, const char* value);

/**
 * Returns the range index for the specified range (in meters).
 * @param[in] scan - the scan
 * @param[in] r - the range
 * @return -1 on failure, otherwise a index between 0 and nbins
 */
int PolarScan_getRangeIndex(PolarScan_t* scan, double r);

/**
 * Returns the range for the specified range index.
 * Same as rscale * ri. If something goes wrong a negative
 * value will be returned.
 * @param[in] scan - self
 * @param[in] ri - range index
 * @return the range in meters or a negative value upon bad input
 */
double PolarScan_getRange(PolarScan_t* scan, int ri);

/**
 * Returns the azimuth index for the specified azimuth.
 * @param[in] scan - the scan
 * @param[in] a - the azimuth (in radians)
 * @return -1 on failure, otherwise a index between 0 and nrays.
 */
int PolarScan_getAzimuthIndex(PolarScan_t* scan, double a);

/**
 * Sets the value at the specified position
 * @param[in] scan - self
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[in] v - the value
 * @returns 1 on success otherwise 0
 */
int PolarScan_setValue(PolarScan_t* scan, int bin, int ray, double v);

/**
 * Sets the parameter value at the specified position
 * @param[in] scan - self
 * @param[in] quantity - the parameter (MAY NOT BE NULL)
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[in] v - the value
 * @returns 1 on success otherwise 0
 */
int PolarScan_setParameterValue(PolarScan_t* scan, const char* quantity, int bin, int ray, double v);

/**
 * Returns the value at the specified index.
 * @param[in] scan - the scan
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[out] v - the data at the specified index
 * @return the type of data
 */
RaveValueType PolarScan_getValue(PolarScan_t* scan, int bin, int ray, double* v);

/**
 * Returns the parameter value at the specified index.
 * @param[in] scan - self
 * @param[in] quantity - the parameter (MAY NOT be NULL)
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[out] v - the found value
 */
RaveValueType PolarScan_getParameterValue(PolarScan_t* scan, const char* quantity, int bin, int ray, double* v);

/**
 * Returns the linear converted value at the specified index. That is,
 * offset + gain * value;
 * @param[in] scan - the scan
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[out] v - the data at the specified index
 * @return the type of data
 */
RaveValueType PolarScan_getConvertedValue(PolarScan_t* scan, int bin, int ray, double* v);

/**
 * Returns the linear converted parameter value at the specified index. That is,
 * offset + gain * value;
 * @param[in] scan - the scan
 * @param[in] quantity - the parameter (MAY NOT BE NULL)
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[out] v - the data at the specified index
 * @return the type of data
 */
RaveValueType PolarScan_getConvertedParameterValue(PolarScan_t* scan, const char* quantity, int bin, int ray, double* v);

/**
 * Returns the bin and ray index from a specified azimuth and range.
 * @param[in] scan - self (MAY NOT BE NULL)
 * @param[in] a - the azimuth (in radians)
 * @param[in] r - the range (in meters)
 * @param[out] ray - the ray index (MAY NOT BE NULL)
 * @param[out] bin - the bin index (MAY NOT BE NULL)
 * @returns 1 on success, otherwise 0 and in that case, bin and ray can not be relied on.
 */
int PolarScan_getIndexFromAzimuthAndRange(PolarScan_t* scan, double a, double r, int* ray, int* bin);

/**
 * Calculates the azimuth and range from bin and ray index.
 * @param[in] scan - self
 * @param[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[out] a - azimuth
 * @param[out] r - range
 * @returns 1 on success otherwise 0
 */
int PolarScan_getAzimuthAndRangeFromIndex(PolarScan_t* scan, int bin, int ray, double* a, double* r);

/**
 * Gets the value at the provided azimuth and range.
 * @param[in] scan - the scan
 * @param[in] a - the azimuth (in radians)
 * @param[in] r - the range (in meters)
 * @param[out] v - the value
 * @return a rave value type
 */
RaveValueType PolarScan_getValueAtAzimuthAndRange(PolarScan_t* scan, double a, double r, double* v);

/**
 * Returns the parameter value at the specified azimuth and range
 * @param[in] scan - self
 * @param[in] quantity - the parameter name
 * @param[in] a - the azimuth (in radians)
 * @param[in] r - the range (in meters)
 * @param[out] v - the value
 * @returns a rave value type (if scan does not contain specified parameter, RaveValueType_UNDEFINED will be returned).
 */
RaveValueType PolarScan_getParameterValueAtAzimuthAndRange(PolarScan_t* scan, const char* quantity, double a, double r, double* v);

/**
 * Returns the converted value at the specified azimuth and range
 * @param[in] scan - self
 * @param[in] quantity - the parameter name
 * @param[in] a - the azimuth (in radians)
 * @param[in] r - the range (in meters)
 * @param[out] v - the value
 * @returns a rave value type (if scan does not contain specified parameter, RaveValueType_UNDEFINED will be returned).
 */
RaveValueType PolarScan_getConvertedParameterValueAtAzimuthAndRange(PolarScan_t* scan, const char* quantity, double a, double r, double* v);

/**
 * Returns the navigation information that is the result from finding the lon/lat-coordinate
 * for this scan.
 * @param[in] scan - self
 * @param[in] lon - the longitude (in radians)
 * @param[in] lat - the latitude (in radians)
 * @param[in,out] info - the navigation information
 */
void PolarScan_getLonLatNavigationInfo(PolarScan_t* scan, double lon, double lat, PolarNavigationInfo* info);

/**
 * Calculates range and elevation index from the azimuth and range
 * in the info object.
 * @param[in] scan - self
 * @param[in,out] info - Will use info.azimuth and info.range to calculate info.ai and info.ri
 * @return 1 if indexes are in range, otherwise 0
 */
int PolarScan_fillNavigationIndexFromAzimuthAndRange(
  PolarScan_t* scan, PolarNavigationInfo* info);

/**
 * Returns the nearest value to the specified longitude, latitude.
 * @param[in] scan - the scan
 * @param[in] lon  - the longitude (in radians)
 * @param[in] lat  - the latitude  (in radians)
 * @param[out] v - the found value
 * @returns a rave value type
 */
RaveValueType PolarScan_getNearest(PolarScan_t* scan, double lon, double lat, double* v);

/**
 * Returns the nearest parameter value to the specified longitude, latitude.
 * @param[in] scan - the scan
 * @param[in] quantity - the quantity
 * @param[in] lon  - the longitude (in radians)
 * @param[in] lat  - the latitude  (in radians)
 * @param[out] v - the found value
 * @returns a rave value type
 */
RaveValueType PolarScan_getNearestParameterValue(PolarScan_t* scan, const char* quantity, double lon, double lat, double* v);

/**
 * Returns the nearest converted parameter value to the specified longitude, latitude.
 * @param[in] scan - the scan
 * @param[in] quantity - the quantity
 * @param[in] lon  - the longitude (in radians)
 * @param[in] lat  - the latitude  (in radians)
 * @param[out] v - the found value
 * @param[in,out] navinfo - the navigation information (may be NULL)
 * @returns a rave value type
 */
RaveValueType PolarScan_getNearestConvertedParameterValue(PolarScan_t* scan, const char* quantity, double lon, double lat, double* v, PolarNavigationInfo* navinfo);

/**
 * Returns the nearest index to the specified long/lat pair.
 * @param[in] scan - self
 * @param[in] lon - the longitude (in radians)
 * @param[in] lat - the latitude (in radians)
 * @param[out] bin - the bin index (MAY NOT BE NULL)
 * @param[out] ray - the ray index (MAY NOT BE NULL)
 * @returns 0 if either bin and/or ray is outside boundaries, otherwise 1
 */
int PolarScan_getNearestIndex(PolarScan_t* scan, double lon, double lat, int* bin, int* ray);

/**
 * Calculates the lon / lat from the index with the adjusted elevation angle.
 * Beamwidth will also be taken into account. Which means that the maximum position
 * will be on the edges of the beam.
 * @param[in] scan - self
 * @þaram[in] bin - the bin index
 * @param[in] ray - the ray index
 * @param[out] lon - the longitude in radians
 * @param[out] lat - the latitude in radians
 * @returns 1 on success otherwise 0
 */
int PolarScan_getLonLatFromIndex(PolarScan_t* scan, int bin, int ray, double* lon, double* lat);

/**
 * Returns the quality value for the quality field that has a name matching the how/task attribute
 * in the list of fields. It will first search the parameter for the quality field, then it
 * will search the scan for the quality field if no field is found it will return 0.
 *
 * @param[in] scan - self
 * @param[in] quantity - the parameter quantity
 * @param[in] ri - the range index (bin)
 * @param[in] ai - the azimuth index (ray)
 * @param[in] name - the value of the how/task attribute
 * @param[out] v - the found value
 * @return 1 if value found otherwise 0
 */
int PolarScan_getQualityValueAt(PolarScan_t* scan, const char* quantity, int ri, int ai, const char* name, double* v);

/**
 * Verifies that all preconditions are met in order to perform
 * a transformation.
 * @param[in] scan - the polar scan
 * @returns 1 if the polar scan is ready, otherwise 0.
 */
int PolarScan_isTransformable(PolarScan_t* scan);

/**
 * Adds a rave attribute to the scan.
 * @param[in] scan - self
 * @param[in] attribute - the attribute
 * @return 1 on success otherwise 0
 */
int PolarScan_addAttribute(PolarScan_t* scan, RaveAttribute_t* attribute);

/**
 * Returns the rave attribute that is named accordingly.
 * @param[in] scan - self
 * @param[in] name - the name of the attribute
 * @returns the attribute if found otherwise NULL
 */
RaveAttribute_t* PolarScan_getAttribute(PolarScan_t* scan, const char* name);

/**
 * Returns a list of attribute names. Release with \@ref #RaveList_freeAndDestroy.
 * @param[in] scan - self
 * @returns a list of attribute names
 */
RaveList_t* PolarScan_getAttributeNames(PolarScan_t* scan);

/**
 * Returns a list of attribute values belonging to this scan.
 * @param[in] scan - self
 * @returns a list of RaveAttributes.
 */
RaveObjectList_t* PolarScan_getAttributeValues(PolarScan_t* scan);

/**
 * Validates the scan can be seen to be valid regarding storage.
 * @param[in] scan - self
 * @param[in] otype - the object type this scan should be accounted for
 * @returns 1 if valid, otherwise 0
 */
int PolarScan_isValid(PolarScan_t* scan, Rave_ObjectType otype);

/**
 * Framework internal function for setting the beamwidth in a scan,
 * used to indicate that the beamwidth comes from the polar volume.
 * I.E. DO NOT USE UNLESS YOU KNOW WHY.
 * @param[in] scan - self
 * @param[in] bw - the beam width in radians
 */
void PolarScanInternal_setPolarVolumeBeamwidth(PolarScan_t* scan, double bw);

/**
 * Returns if the beamwidth comes from a volume or not.
 * I.E. DO NOT USE UNLESS YOU KNOW WHY.
 * @param[in] scan - self
 * @returns -1 if default setting, 1 if beamwidth comes from a polar volume otherwise 0
 */
int PolarScanInternal_isPolarVolumeBeamwidth(PolarScan_t* scan);

#endif
