/**
 ******************************************************************************
 * @addtogroup TauLabsModules Tau Labs Modules
 * @{
 * @addtogroup Sensors
 * @{
 *
 * @file       sensors.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
 * @brief      Acquire sensor data from sensors registered with @ref PIOS_Sensors
 *
 * @see        The GNU Public License (GPL) Version 3
 *
 ******************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "openpilot.h"
#include "pios.h"
#include "physical_constants.h"

// UAVOs
#include "sonaraltitude.h"

// Private constants
#define STACK_SIZE_BYTES 1000
#define TASK_PRIORITY (tskIDLE_PRIORITY+3)
#define SENSOR_PERIOD 6		// this allows sensor data to arrive as slow as 166Hz
#define REQUIRED_GOOD_CYCLES 50

// Private types
enum mag_calibration_algo {
	MAG_CALIBRATION_PRELEMARI,
	MAG_CALIBRATION_NORMALIZE_LENGTH
};

// Private functions
static void AltitudeTask(void *parameters);

static void update_sonar(void);

// Private variables
static xTaskHandle altitudeTaskHandle;

/**
 * Initialise the module.  Called before the start function
 * \returns 0 on success or -1 if initialisation failed
 */
static int32_t AltitudeInitialize(void)
{
	SonarAltitudeInitialize();
	//AttitudeSettingsConnectCallback(&settingsUpdatedCb);
	return 0;
}

/**
 * Start the task.  Expects all objects to be initialized by this point.
 * \returns 0 on success or -1 if initialisation failed
 */
static int32_t AltitudeStart(void)
{
	// Start main task
	xTaskCreate(AltitudeTask, (signed char *)"Sonar", STACK_SIZE_BYTES/4, NULL, TASK_PRIORITY, &altitudeTaskHandle);
	//TaskMonitorAdd(TASKINFO_RUNNING_SENSORS, sensorsTaskHandle);

	return 0;
}

MODULE_INITCALL(AltitudeInitialize, AltitudeStart)


/**
 * The sensor task.  This polls the gyros at 500 Hz and pumps that data to
 * stabilization and to the attitude loop
 */
static void AltitudeTask(void *parameters)
{
	// Main task loop

	portTickType lastSysTime = xTaskGetTickCount();

	while (1) {
		update_sonar();
		vTaskDelayUntil(&lastSysTime, MS2TICKS(SENSOR_PERIOD));
	}
}

/**
 * @brief Apply calibration and rotation to the raw gyro data
 * @param[in] gyros The raw gyro data
 */
static void update_sonar(void)
{
	SonarAltitudeData sonar_data;
	sonar_data.Altitude = 10.0;
	SonarAltitudeSet(&sonar_data);
	return;
}

/**
  * @}
  * @}
  */
