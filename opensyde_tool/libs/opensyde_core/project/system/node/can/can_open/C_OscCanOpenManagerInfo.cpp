//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager general data information

   Data class for CANopen manager general data information

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscCanOpenManagerInfo.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

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
C_OscCanOpenManagerInfo::C_OscCanOpenManagerInfo(void) :
   q_UseOpenSydeNodeId(true),
   u8_NodeIdValue(0),
   u16_GlobalSdoTimeoutMs(100),
   q_AutostartCanOpenManager(true),
   q_StartDevices(true),
   q_NmtStartAll(false),
   e_NmtErrorBehaviour(eRESTART_ALL_DEVICES),
   q_EnableHeartbeatProducing(true),
   u16_HeartbeatProducerTimeMs(100),
   q_ProduceSyncMessage(false),
   u32_SyncCyclePeriodUs(20000UL),
   u32_SyncWindowLengthUs(10000UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerInfo::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_UseOpenSydeNodeId, sizeof(this->q_UseOpenSydeNodeId),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_NodeIdValue, sizeof(this->u8_NodeIdValue),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_GlobalSdoTimeoutMs, sizeof(this->u16_GlobalSdoTimeoutMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_AutostartCanOpenManager, sizeof(this->q_AutostartCanOpenManager),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_StartDevices, sizeof(this->q_StartDevices),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_NmtStartAll, sizeof(this->q_NmtStartAll),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_NmtErrorBehaviour, sizeof(this->e_NmtErrorBehaviour),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_EnableHeartbeatProducing, sizeof(this->q_EnableHeartbeatProducing),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_HeartbeatProducerTimeMs, sizeof(this->u16_HeartbeatProducerTimeMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_ProduceSyncMessage,
                                       sizeof(this->q_ProduceSyncMessage),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_SyncCyclePeriodUs, sizeof(this->u32_SyncCyclePeriodUs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_SyncWindowLengthUs, sizeof(this->u32_SyncWindowLengthUs),
                                       oru32_HashValue);
   for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_It =
           this->c_CanOpenDevices.begin();
        c_It != this->c_CanOpenDevices.end(); ++c_It)
   {
      c_It->first.CalcHash(oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for errors of CANopen Manager

   \param[out]     opq_CoNodeIdConflict           Duplicate error with CANopen Node ID detected
   \param[out]     opq_CoManagerNodeIdInvalid     Invalid error with CANopen Node ID of Manager detected
   \param[out]     opq_CoDevicesNodeIdInvalid     Invalid error with CANopen Node ID of at least one Device detected
   \param[out]     opq_HearbeatTimeInvalid        Error with hearbeat time of Manager detected
   \param[in]      oq_CheckDeviceToDeviceErrors   true: run error checks between devices and between manager and devices
                                                  false: run error checks between manager and device only
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerInfo::CheckErrorManager(bool * const opq_CoNodeIdConflict,
                                                bool * const opq_CoManagerNodeIdInvalid,
                                                bool * const opq_CoDevicesNodeIdInvalid,
                                                bool * const opq_HearbeatTimeInvalid,
                                                const bool oq_CheckDeviceToDeviceErrors) const
{
   std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice;

   if (opq_CoManagerNodeIdInvalid != NULL)
   {
      // Check for the valid range of the manager itself
      if ((this->u8_NodeIdValue > 0U) &&
          (this->u8_NodeIdValue <= 127))
      {
         *opq_CoManagerNodeIdInvalid = false;
      }
      else
      {
         *opq_CoManagerNodeIdInvalid = true;
      }
   }

   // Check the devices related errors
   if ((opq_CoNodeIdConflict != NULL) || (opq_CoDevicesNodeIdInvalid != NULL))
   {
      bool q_TempCoNodeIdConflict = false;
      bool q_TempCoDevicesNodeIdInvalid = false;
      bool * pq_TempCoNodeIdConflict = NULL;
      bool * pq_TempCoDevicesNodeIdInvalid = NULL;

      // Using temporary pointers to beware the optional character of the output parameters
      if (opq_CoNodeIdConflict != NULL)
      {
         pq_TempCoNodeIdConflict = &q_TempCoNodeIdConflict;
         *opq_CoNodeIdConflict = false;
      }
      if (opq_CoDevicesNodeIdInvalid != NULL)
      {
         pq_TempCoDevicesNodeIdInvalid = &q_TempCoDevicesNodeIdInvalid;
         *opq_CoDevicesNodeIdInvalid = false;
      }

      // Check all devices
      for (c_ItDevice = this->c_CanOpenDevices.begin(); c_ItDevice != this->c_CanOpenDevices.end(); ++c_ItDevice)
      {
         tgl_assert(this->CheckErrorDeviceCoNodeId(c_ItDevice->first, pq_TempCoNodeIdConflict,
                                                   pq_TempCoDevicesNodeIdInvalid,
                                                   oq_CheckDeviceToDeviceErrors) == C_NO_ERR);

         // Transfer to the output pointer only true result to not overwrite previous detections
         if ((opq_CoNodeIdConflict != NULL) &&
             (q_TempCoNodeIdConflict == true))
         {
            *opq_CoNodeIdConflict = q_TempCoNodeIdConflict;
         }
         if ((opq_CoDevicesNodeIdInvalid != NULL) &&
             (q_TempCoDevicesNodeIdInvalid == true))
         {
            *opq_CoDevicesNodeIdInvalid = q_TempCoDevicesNodeIdInvalid;
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
   \param[out]     opq_CoNodeIdConflict           Duplicate error with CANopen Node ID detected
   \param[out]     opq_CoNodeIdInvalid            Invalid error with CANopen Node ID detected (only 1 - 127 are allowed)
   \param[in]      oq_CheckDeviceToDeviceErrors   true: run error checks between devices and between manager and devices
                                                  false: run error checks between manager and device only

   \retval   C_NO_ERR   Error check successful
   \retval   C_RANGE    Parameters invalid, error check not executed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenManagerInfo::CheckErrorDeviceCoNodeId(const C_OscCanInterfaceId & orc_DeviceId,
                                                          bool * const opq_CoNodeIdConflict,
                                                          bool * const opq_CoNodeIdInvalid,
                                                          const bool oq_CheckDeviceToDeviceErrors) const
{
   int32_t s32_Return = C_RANGE;

   const std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
      this->c_CanOpenDevices.find(orc_DeviceId);

   if (c_ItDevice != this->c_CanOpenDevices.end())
   {
      const C_OscCanOpenManagerDeviceInfo & rc_DevInfo = c_ItDevice->second;

      s32_Return = C_NO_ERR;

      if (opq_CoNodeIdConflict != NULL)
      {
         *opq_CoNodeIdConflict = false;

         if (this->u8_NodeIdValue == rc_DevInfo.u8_NodeIdValue)
         {
            // Same as the manager -> error
            *opq_CoNodeIdConflict = true;
         }
         else if (oq_CheckDeviceToDeviceErrors == true)
         {
            std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDeviceComp;

            // Compare with all other devices
            for (c_ItDeviceComp = this->c_CanOpenDevices.begin(); c_ItDeviceComp != this->c_CanOpenDevices.end();
                 ++c_ItDeviceComp)
            {
               if (c_ItDevice != c_ItDeviceComp)
               {
                  const C_OscCanOpenManagerDeviceInfo & rc_DevInfoCompare = c_ItDeviceComp->second;

                  if (rc_DevInfo.u8_NodeIdValue == rc_DevInfoCompare.u8_NodeIdValue)
                  {
                     // error detected
                     *opq_CoNodeIdConflict = true;
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

      if (opq_CoNodeIdInvalid != NULL)
      {
         // Check for the valid range
         if ((rc_DevInfo.u8_NodeIdValue > 0U) &&
             (rc_DevInfo.u8_NodeIdValue <= 127))
         {
            *opq_CoNodeIdInvalid = false;
         }
         else
         {
            *opq_CoNodeIdInvalid = true;
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
int32_t C_OscCanOpenManagerInfo::CheckErrorDeviceHeartbeat(const C_OscCanInterfaceId & orc_DeviceId,
                                                           bool * const opq_HearbeatTimeInvalid) const
{
   int32_t s32_Return = C_RANGE;

   const std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
      this->c_CanOpenDevices.find(orc_DeviceId);

   if ((c_ItDevice != this->c_CanOpenDevices.end()) &&
       (opq_HearbeatTimeInvalid != NULL))
   {
      const C_OscCanOpenManagerDeviceInfo & rc_DevInfo = c_ItDevice->second;

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
