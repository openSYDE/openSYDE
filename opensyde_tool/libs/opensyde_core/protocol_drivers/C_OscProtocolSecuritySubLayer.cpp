//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE protocol security sub layer

   For details cf. documentation in .h file.

   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscProtocolSecuritySubLayer.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
std::map<C_OscProtocolDriverOsyNode, C_OscProtocolSecuritySubLayer> C_OscProtocolSecuritySubLayer::mhc_TheConfig;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Initialize with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolSecuritySubLayerBase::C_OscProtocolSecuritySubLayerBase() :
   mq_EncryptionIsActive(false),
   mq_EcdhKeysInitialized(false),
   mq_AesKeyDerived(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   Clean up
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolSecuritySubLayerBase::~C_OscProtocolSecuritySubLayerBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Initialize with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolSecuritySubLayer::C_OscProtocolSecuritySubLayer() :
   C_OscProtocolSecuritySubLayerBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   Clean up
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolSecuritySubLayer::~C_OscProtocolSecuritySubLayer()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get config with specified NodeId

   If no entry exists add one to our map.
   So we keep a static map with multiple known busIds + nodeIds and their current traffic encryption configuration.
   The combination of busId + nodeId is guaranteed to be unique on a valid system.
   Known nodes and their states can be cleaned up with h_ClearAll.

   \param[in]    orc_NodeId   node id to get config for

   \return  pointer to config
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscProtocolSecuritySubLayer * C_OscProtocolSecuritySubLayer::h_GetConfigByNodeId(
   const C_OscProtocolDriverOsyNode & orc_NodeId)
{
   //either get existing entry define for that node id or create new (empty) one
   return &mhc_TheConfig[orc_NodeId];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear all known nodes

   Clear list of all known nodes from map.
   Beware that this will render previous pointer returned by h_GetConfigByNodeId invalid.
   So only call this at central spots where there is no risk of the pointers still being in use.

   Effectively this will result in all new nodes being (re-)added when calling h_GetConfigByNodeId.
   So they will start with uninitialized keys and encryption off.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProtocolSecuritySubLayer::h_ClearAll()
{
   mhc_TheConfig.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize ECDH keys

   If not done so already create set of private and public ECDH keys and store in class fields.

   \return
   \retval  C_NO_ERR    keys create
   \retval  C_NOACT     could not create keys
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayer::InitEcdhKeys()
{
   int32_t s32_Result = C_NO_ERR;

   //once per life time of instance
   if (mq_EcdhKeysInitialized == false)
   {
      s32_Result = mc_Ecdh.CreateEcKeys(mau8_EcdhPublicKey);
      if (s32_Result == C_NO_ERR)
      {
         this->mq_EcdhKeysInitialized = true;
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get created ECDH public key

   If no key was created yet the function will try to do so.

   \param[out]    orc_TrafficEncryptionPublicClientKey   ECDH public key in compressed format

   \return
   \retval  C_NO_ERR    key provided
   \retval  C_NOACT     no key available; trying to create failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayer::GetEcdhPublicKey(std::vector<uint8_t> & orc_TrafficEncryptionPublicClientKey)
{
   const int32_t s32_Result = this->InitEcdhKeys();

   if (s32_Result == C_NO_ERR)
   {
      orc_TrafficEncryptionPublicClientKey.resize(C_OscSecurityEcdhAes::hu32_PUBLIC_KEY_LENGTH);
      (void)std::memcpy(&orc_TrafficEncryptionPublicClientKey[0], &this->mau8_EcdhPublicKey[0],
                        C_OscSecurityEcdhAes::hu32_PUBLIC_KEY_LENGTH);
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Derive AES key

   Derive AES from own private key and other's public key.
   Store result in class field.
   Can be called multiple times after ECDH key initialization to create new shared AES key.

   \param[in]   orc_OthersPublicKey    Others public key

   \return
   \retval   C_NO_ERR   success, key is orau8_AesKey
   \retval   C_RANGE    invalid parameter
   \retval   C_NOACT    could not derive key
   \retval   C_CONFIG   own ECDH keys were not initialized
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayer::DeriveAesKey(const std::vector<uint8_t> & orc_OthersPublicKey)
{
   int32_t s32_Result = C_CONFIG;

   if (mq_EcdhKeysInitialized == true)
   {
      s32_Result = C_RANGE;
      if (orc_OthersPublicKey.size() == C_OscSecurityEcdhAes::hu32_PUBLIC_KEY_LENGTH)
      {
         uint8_t au8_OthersKey[C_OscSecurityEcdhAes::hu32_PUBLIC_KEY_LENGTH];
         (void)std::memcpy(&au8_OthersKey[0], &orc_OthersPublicKey[0], C_OscSecurityEcdhAes::hu32_PUBLIC_KEY_LENGTH);
         s32_Result = mc_Ecdh.DeriveAesKey(au8_OthersKey);
         if (s32_Result == C_NO_ERR)
         {
            mq_AesKeyDerived = true;
         }
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set AES init vector

   Set init vector for the the AES-CBC encryption/decryption

   \param[out]    orc_InitVector   AES init vector

   \return
   \retval  C_NO_ERR    vector set
   \retval  C_RANGE     invalid vector
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayer::SetAesInitVector(const std::vector<uint8_t> & orc_InitVector)
{
   int32_t s32_Result = C_RANGE;

   if (orc_InitVector.size() == C_OscSecurityAesCbc::hu32_IV_LENGTH)
   {
      (void)std::memcpy(&this->mau8_AesInitVectorRequest[0], &orc_InitVector[0], C_OscSecurityAesCbc::hu32_IV_LENGTH);
      (void)std::memcpy(&this->mau8_AesInitVectorResponse[0], &orc_InitVector[0], C_OscSecurityAesCbc::hu32_IV_LENGTH);
      s32_Result = C_NO_ERR;
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set flag to activate/deactivate encryption

   Flag is just "pass through" in this class.

   \param[out]    oq_IsActive    true: mark encryption as active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscProtocolSecuritySubLayerBase::SetEncryptionIsActive(const bool oq_IsActive)
{
   this->mq_EncryptionIsActive = oq_IsActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get flag to check active encryption

   Flag is just "pass through" in this class.

   \return   true: encryption flag is active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscProtocolSecuritySubLayerBase::GetEncryptionIsActive() const
{
   return this->mq_EncryptionIsActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Perform encryption

   Uses known AES key and request init vector.

   \param[in]     orc_Input       data to encrypt
   \param[out]    orc_Output      encrypted data

   \return
   \retval C_NO_ERR     no problems
   \retval C_CONFIG     encryption engine not initialized or not set active
   \retval C_CHECKSUM   encryption failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayer::m_Encrypt(const std::vector<uint8_t> & orc_Input,
                                                 std::vector<uint8_t> & orc_Output)
{
   return mc_Ecdh.AesEncrypt(this->mau8_AesInitVectorRequest, orc_Input, orc_Output);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Perform decryption

   Uses known AES key and response init vector.

   \param[in]     orc_Input       data to decrypt
   \param[out]    orc_Output      decrypted data

   \return
   \retval C_NO_ERR     no problems
   \retval C_CONFIG     decryption engine not initialized or not set active
   \retval C_CHECKSUM   decryption failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayer::m_Decrypt(const std::vector<uint8_t> & orc_Input,
                                                 std::vector<uint8_t> & orc_Output)
{
   return mc_Ecdh.AesDecrypt(this->mau8_AesInitVectorResponse, orc_Input, orc_Output);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Encrypt and wrap up service

   Encrypt service data and wrap up into a SecuredDataTransmission service.

   \param[in]     orc_UnwrappedService    unencrypted service to wrap up
   \param[out]    orc_WrappedService      wrapped up, encrypted service

   \return
   \retval C_NO_ERR     no problems
   \retval C_CONFIG     encryption engine not initialized or not set active
   \retval C_CHECKSUM   encryption failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayerBase::WrapRequest(const C_OscProtocolDriverOsyService & orc_UnwrappedService,
                                                       C_OscProtocolDriverOsyService & orc_WrappedService)
{
   int32_t s32_Result = C_CONFIG;

   if ((this->mq_EncryptionIsActive == true) && (this->mq_AesKeyDerived == true))
   {
      std::vector<uint8_t> c_EncryptedData;
      s32_Result = m_Encrypt(orc_UnwrappedService.c_Data, c_EncryptedData);
      if (s32_Result == C_NO_ERR)
      {
         orc_WrappedService.q_CanTransferWithoutFlowControl = false; //this service always needs a flow control

         orc_WrappedService.c_Data.resize(4U + c_EncryptedData.size());
         orc_WrappedService.c_Data[0] = mhu8_OSY_SI_SECURED_DATA_TRANSMISSION;
         orc_WrappedService.c_Data[1] = 0x00U; // 16bit APAR: Bit0 is set (request message); other bits are constants
         orc_WrappedService.c_Data[2] = 0x19U;
         orc_WrappedService.c_Data[3] = 0x00U; // algorithm: AES-CBC-128 with PKCS#7
         (void)std::memcpy(&orc_WrappedService.c_Data[4], &c_EncryptedData[0], c_EncryptedData.size());

         tgl_assert(c_EncryptedData.size() >= C_OscSecurityAesCbc::hu32_IV_LENGTH);

         //remember last 16 bytes of encrypted result as new init vector for following services:
         (void)std::memcpy(&this->mau8_AesInitVectorRequest[0],
                           &c_EncryptedData[c_EncryptedData.size() - C_OscSecurityAesCbc::hu32_IV_LENGTH],
                           C_OscSecurityAesCbc::hu32_IV_LENGTH);
      }
      else
      {
         s32_Result = C_CHECKSUM;
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Unwrap and decrypt service response

   Unwrap incoming SecuredDataTransmission response and decrypt payload.

   \param[in]     orc_WrappedService    encrypted service to unwrap
   \param[out]    orc_UnwrappedService  unwrapped, decrypted service

   \return
   \retval C_NO_ERR     no problems
   \retval C_RANGE      orc_WrappedService is not a valid SecuredDataTransmission service
   \retval C_CONFIG     encryption engine not initialized or not set active
   \retval C_CHECKSUM   decryption failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscProtocolSecuritySubLayerBase::UnwrapResponse(const C_OscProtocolDriverOsyService & orc_WrappedService,
                                                          C_OscProtocolDriverOsyService & orc_UnwrappedService)
{
   int32_t s32_Result = C_CONFIG;

   if ((this->mq_EncryptionIsActive == true) && (this->mq_AesKeyDerived == true))
   {
      s32_Result = C_RANGE;

      //Encrypted data must have a size that is a multiple of 16
      //We need at least one block of data to work on.
      if ((orc_WrappedService.c_Data.size() >= (4U + C_OscSecurityAesCbc::hu32_IV_LENGTH)) &&
          (orc_WrappedService.c_Data[0] == (mhu8_OSY_SI_SECURED_DATA_TRANSMISSION | 0x40U)) &&
          (orc_WrappedService.c_Data[1] == 0x00U) && //APAR HB
          (orc_WrappedService.c_Data[2] == 0x18U) && //APAR LB (is response?)
          (orc_WrappedService.c_Data[3] == 0x00U))   //algorithm == AES-CBC-128 with PKCS#7
      {
         //We have a SecuredDataTransmission response. Yeah. Try to decrypt content.
         std::vector<uint8_t> c_EncryptedData;
         c_EncryptedData.resize(orc_WrappedService.c_Data.size() - 4);
         (void)std::memcpy(&c_EncryptedData[0], &orc_WrappedService.c_Data[4], c_EncryptedData.size());

         s32_Result = m_Decrypt(c_EncryptedData, orc_UnwrappedService.c_Data);
         if (s32_Result == C_NO_ERR)
         {
            //remember last 16 bytes of encrypted data as new init vector for following services:
            (void)std::memcpy(&this->mau8_AesInitVectorResponse[0],
                              &c_EncryptedData[c_EncryptedData.size() - C_OscSecurityAesCbc::hu32_IV_LENGTH],
                              C_OscSecurityAesCbc::hu32_IV_LENGTH);
         }
         else
         {
            s32_Result = C_CHECKSUM;
         }
      }
   }

   return s32_Result;
}
