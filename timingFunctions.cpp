/* timingFunctions.cpp
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


#include "timingFunctions.h"

std::ofstream profile_csv_file("profile.csv");

goodclock get_start_time () 
{
    goodclock startTime = std::chrono::high_resolution_clock::now();
    return startTime;
}

void save_time_to_csv (goodclock startTime, 
                       std::string funcName) 
{
    auto const endTime = std::chrono::high_resolution_clock::now();
    auto const elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>> (endTime - startTime);

    profile_csv_file << funcName << "," << elapsedTime.count() << "\n";
}

