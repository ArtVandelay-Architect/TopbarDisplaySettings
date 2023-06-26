/* timingFunctions.h
 *
 * Copyright 2023 Luke Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <chrono>
#include <fstream>

#ifndef TIMINGFUNCTIONS_H
#define TIMINGFUNCTIONS_H

typedef std::chrono::time_point<std::chrono::high_resolution_clock> goodclock;

goodclock get_start_time   ();

//Save the time taken since start time in csv file in seconds
//To get startTime, use
//auto const startTime = std::chrono::high_resolution_clock::now();
void      save_time_to_csv (goodclock startTime, 
                            std::string funcName);

#endif /* TIMINGFUNCTIONS_H */
