/*
 * ExtendedAircraft.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef EXTENDEDAIRCRAFT_H_
#define EXTENDEDAIRCRAFT_H_

#include "Aircraft.h"

class ExtendedAircraft: public Aircraft
{
public:
   ExtendedAircraft();
   virtual ~ExtendedAircraft();

   float track;
   int ground_speed;
   std::string comment;
   int address_type;
   int aircraft_type;
   int climb_rate;
};

#endif /* EXTENDEDAIRCRAFT_H_ */
