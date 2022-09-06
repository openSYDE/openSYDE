//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager general data information

   Data class for CANopen manager general data information

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "CSCLChecksums.h"
#include "C_OSCCanOpenManagerInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanOpenManagerInfo::C_OSCCanOpenManagerInfo(void) :
   q_UseOpenSYDENodeID(true),
   u8_NodeIDValue(0),
   u16_GlobalSDOTimeoutMs(100),
   q_AutostartCanOpenManager(true),
   q_StartDevices(true),
   q_NMTStartAll(false),
   e_NMTErrorBehaviour(eRESTART_ALL_DEVICES),
   q_EnableHeartbeatProducing(true),
   u16_HeartbeatProducerTimeMs(100)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenManagerInfo::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_UseOpenSYDENodeID, sizeof(this->q_UseOpenSYDENodeID),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_NodeIDValue, sizeof(this->u8_NodeIDValue),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_GlobalSDOTimeoutMs, sizeof(this->u16_GlobalSDOTimeoutMs),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_AutostartCanOpenManager, sizeof(this->q_AutostartCanOpenManager),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_StartDevices, sizeof(this->q_StartDevices),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_NMTStartAll, sizeof(this->q_NMTStartAll),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_NMTErrorBehaviour, sizeof(this->e_NMTErrorBehaviour),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_EnableHeartbeatProducing, sizeof(this->q_EnableHeartbeatProducing),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_HeartbeatProducerTimeMs, sizeof(this->u16_HeartbeatProducerTimeMs),
                                      oru32_HashValue);
   for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_It =
           this->c_CanOpenDevices.begin();
        c_It != this->c_CanOpenDevices.end(); ++c_It)
   {
      c_It->first.CalcHash(oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for errors of CANopen Manager

   \param[out]     opq_CoNodeIDConflict           Duplicate error with CANopen Node ID detected
   \param[out]     opq_CoManagerNodeIDInvalid     Invalid error with CANopen Node ID of Manager detected
   \param[out]     opq_CoDevicesNodeIDInvalid     Invalid error with CANopen Node ID of at least one Device detected
   \param[out]     opq_HearbeatTimeInvalid        Error with hearbeat time of Manager detected
   \param[in]      oq_CheckDeviceToDeviceErrors   true: run error checks between devices and between manager and devices
                                                  false: run error checks between manager and device only
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenManagerInfo::CheckErrorManager(bool * const opq_CoNodeIDConflict,
                                                bool * const opq_CoManagerNodeIDInvalid,
                                                bool * const opq_CoDevicesNodeIDInvalid,
                                                bool * const opq_HearbeatTimeInvalid,
                                                const bool oq_CheckDeviceToDeviceErrors) const
{
   std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice;

   if (opq_CoManagerNodeIDInvalid != NULL)
   {
      // Check for the valid range of the manager itself
      if ((this->u8_NodeIDValue > 0U) &&
          (this->u8_NodeIDValue <= 127))
      {
         *opq_CoManagerNodeIDInvalid = false;
      }
      else
      {
         *opq_CoManagerNodeIDInvalid = true;
      }
   }

   // Check the devices related errors
   if ((opq_CoNodeIDConflict != NULL) || (opq_CoDevicesNodeIDInvalid != NULL))
   {
      bool q_TempCoNodeIDConflict = false;
      bool q_TempCoDevicesNodeIDInvalid = false;
      bool * pq_TempCoNodeIDConflict = NULL;
      bool * pq_TempCoDevicesNodeIDInvalid = NULL;

      // Using temporary pointers to beware the optional character of the output parameters
      if (opq_CoNodeIDConflict != NULL)
      {
         pq_TempCoNodeIDConflict = &q_TempCoNodeIDConflict;
         *opq_CoNodeIDConflict = false;
      }
      if (opq_CoDevicesNodeIDInvalid != NULL)
      {
         pq_TempCoDevicesNodeIDInvalid = &q_TempCoDevicesNodeIDInvalid;
         *opq_CoDevicesNodeIDInvalid = false;
      }

      // Check all devices
      for (c_ItDevice = this->c_CanOpenDevices.begin(); c_ItDevice != this->c_CanOpenDevices.end(); ++c_ItDevice)
      {
         tgl_assert(this->CheckErrorDeviceCoNodeId(c_ItDevice->first, pq_TempCoNodeIDConflict,
                                                   pq_TempCoDevicesNodeIDInvalid,
                                                   oq_CheckDeviceToDeviceErrors) == C_NO_ERR);

         // Transfer to the output pointer only true result to not overwrite previous detections
         if ((opq_CoNodeIDConflict != NULL) &&
             (q_TempCoNodeIDConflict == true))
         {
            *opq_CoNodeIDConflict = q_TempCoNodeIDConflict;
         }
         if ((opq_CoDevicesNodeIDInvalid != NULL) &&
             (q_TempCoDevicesNodeIDInvalid == true))
         {
            *opq_CoDevicesNodeIDInvalid = q_TempCoDevicesNodeIDInvalid;
         }
      }
   }

   if (opq_HearbeatTimeInvalid != NULL)
   {
      *opq_HearbeatTimeInvalid = false;

      // Check all devices
      for (c_ItDevice = this->c_CanOpenDevices.begin(); c_ItDevice != this->c_CanOpenDevices.end(); ++c_ItDevice)
      {
         tgl_assert(this->CheckErrorDeviceHeartbeat(c_ItDevice->first, opq_HearbeatTimeInvalid) == C_NO_ERR);

         if (*opq_HearbeatTimeInvalid == true)
         {
            // One error found
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a device has an consumer heartbeat error

   \param[in]      orc_DeviceId                   CANopen device ID
   \param[out]     opq_CoNodeIDConflict           Duplicate error with CANopen Node ID detected
   \param[out]     opq_CoNodeIDInvalid            Invalid error with CANopen Node ID detected (only 1 - 127 are allowed)
   \param[in]      oq_CheckDeviceToDeviceErrors   true: run error checks between devices and between manager and devices
                                                  false: run error checks between manager and device only

   \retval   C_NO_ERR   Error check successful
   \retval   C_RANGE    Parameters invalid, error check not executed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerInfo::CheckErrorDeviceCoNodeId(const C_OSCCanInterfaceId & orc_DeviceId,
                                                         bool * const opq_CoNodeIDConflict,
                                                         bool * const opq_CoNodeIDInvalid,
                                                         const bool oq_CheckDeviceToDeviceErrors) const
{
   sint32 s32_Return = C_RANGE;

   const std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
      this->c_CanOpenDevices.find(orc_DeviceId);

   if (c_ItDevice != this->c_CanOpenDevices.end())
   {
      const C_OSCCanOpenManagerDeviceInfo & rc_DevInfo = c_ItDevice->second;

      s32_Return = C_NO_ERR;

      if (opq_CoNodeIDConflict != NULL)
      {
         *opq_CoNodeIDConflict = false;

         if (this->u8_NodeIDValue == rc_DevInfo.u8_NodeIDValue)
         {
            // Same as the manager -> error
            *opq_CoNodeIDConflict = true;
         }
         else if (oq_CheckDeviceToDeviceErrors == true)
         {
            std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDeviceComp;

            // Compare with all other devices
            for (c_ItDeviceComp = this->c_CanOpenDevices.begin(); c_ItDeviceComp != this->c_CanOpenDevices.end();
                 ++c_ItDeviceComp)
            {
               if (c_ItDevice != c_ItDeviceComp)
               {
                  const C_OSCCanOpenManagerDeviceInfo & rc_DevInfoCompare = c_ItDeviceComp->second;

                  if (rc_DevInfo.u8_NodeIDValue == rc_DevInfoCompare.u8_NodeIDValue)
                  {
                     // error detected
                     *opq_CoNodeIDConflict = true;
                     break;
                  }
               }
            }
         }
         else
         {
            // Nothing to do
         }
      }

      if (opq_CoNodeIDInvalid != NULL)
      {
         // Check for the valid range
         if ((rc_DevInfo.u8_NodeIDValue > 0U) &&
             (rc_DevInfo.u8_NodeIDValue <= 127))
         {
            *opq_CoNodeIDInvalid = false;
         }
         else
         {
            *opq_CoNodeIDInvalid = true;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if a device has an consumer heartbeat error

   \param[in]       orc_DeviceId              CANopen device ID
   \param[out]      opq_HearbeatTimeInvalid   Error with Hearbeat time of Manager detected

   \retval   C_NO_ERR   Error check successful
   \retval   C_RANGE    Parameters invalid, error check not executed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenManagerInfo::CheckErrorDeviceHeartbeat(const C_OSCCanInterfaceId & orc_DeviceId,
                                                          bool * const opq_HearbeatTimeInvalid) const
{
   sint32 s32_Return = C_RANGE;

   const std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
      this->c_CanOpenDevices.find(orc_DeviceId);

   if ((c_ItDevice != this->c_CanOpenDevices.end()) &&
       (opq_HearbeatTimeInvalid != NULL))
   {
      const C_OSCCanOpenManagerDeviceInfo & rc_DevInfo = c_ItDevice->second;

      s32_Return = C_NO_ERR;

      *opq_HearbeatTimeInvalid = false;

      // If the heartbeat producing of the manager and consuming of the device is enabled
      // the consumer time must be higher than the producer time to valid
      if ((this->q_EnableHeartbeatProducing == true) &&
          (rc_DevInfo.q_EnableHeartbeatConsuming == true) &&
          (rc_DevInfo.u16_HeartbeatConsumerTimeMs <= this->u16_HeartbeatProducerTimeMs))
      {
         *opq_HearbeatTimeInvalid = true;
      }
   }

   return s32_Return;
}
