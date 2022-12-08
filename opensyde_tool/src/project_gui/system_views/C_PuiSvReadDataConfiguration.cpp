//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for read data element configuration information (implementation)

   Data class for read data element configuration information

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvReadDataConfiguration::C_PuiSvReadDataConfiguration(void) :
   e_TransmissionMode(eTM_ON_TRIGGER),
   u8_RailIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvReadDataConfiguration::~C_PuiSvReadDataConfiguration(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvReadDataConfiguration::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_TransmissionMode, sizeof(this->e_TransmissionMode), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_RailIndex, sizeof(this->u8_RailIndex), oru32_HashValue);

   this->c_ChangeThreshold.CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize default threshold

   \param[in] orc_Min Min value
   \param[in] orc_Max Max value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSvReadDataConfiguration::InitDefaultThreshold(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                                           const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max)
{
   int32_t s32_Retval = C_NO_ERR;

   //Check if format as expected
   if ((orc_Min.GetType() == orc_Max.GetType()) && (orc_Min.GetArray() == orc_Max.GetArray()))
   {
      this->c_ChangeThreshold.SetType(orc_Min.GetType());
      if ((orc_Min.GetArray() == false) && (orc_Max.GetArray() == false))
      {
         this->c_ChangeThreshold.SetArray(false);

         //Each type: init default value by 10% of range -> (1/10)
         switch (orc_Min.GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            {
               const uint8_t u8_Min = orc_Min.GetValueU8();
               const uint8_t u8_Max = orc_Max.GetValueU8();
               this->c_ChangeThreshold.SetValueU8(static_cast<uint8_t>(u8_Max - u8_Min) / static_cast<uint8_t>(10U));
               break;
            }
         case C_OscNodeDataPoolContent::eUINT16:
            {
               const uint16_t u16_Min = orc_Min.GetValueU16();
               const uint16_t u16_Max = orc_Max.GetValueU16();
               this->c_ChangeThreshold.SetValueU16(static_cast<uint16_t>(u16_Max - u16_Min) /
                                                   static_cast<uint16_t>(10U));
               break;
            }
         case C_OscNodeDataPoolContent::eUINT32:
            {
               const uint32_t u32_Min = orc_Min.GetValueU32();
               const uint32_t u32_Max = orc_Max.GetValueU32();
               this->c_ChangeThreshold.SetValueU32((u32_Max - u32_Min) / 10UL);
               break;
            }
         case C_OscNodeDataPoolContent::eUINT64:
            {
               const uint64_t u64_Min = orc_Min.GetValueU64();
               const uint64_t u64_Max = orc_Max.GetValueU64();
               this->c_ChangeThreshold.SetValueU64((u64_Max - u64_Min) / 10ULL);
               break;
            }
         case C_OscNodeDataPoolContent::eSINT8:
            {
               const int8_t s8_Min = orc_Min.GetValueS8();
               const int8_t s8_Max = orc_Max.GetValueS8();
               this->c_ChangeThreshold.SetValueS8((s8_Max - s8_Min) / static_cast<int8_t>(10));
               break;
            }
         case C_OscNodeDataPoolContent::eSINT16:
            {
               const int16_t s16_Min = orc_Min.GetValueS16();
               const int16_t s16_Max = orc_Max.GetValueS16();
               this->c_ChangeThreshold.SetValueS16((s16_Max - s16_Min) / static_cast<int16_t>(10));
               break;
            }
         case C_OscNodeDataPoolContent::eSINT32:
            {
               const int32_t s32_Min = orc_Min.GetValueS32();
               const int32_t s32_Max = orc_Max.GetValueS32();
               this->c_ChangeThreshold.SetValueS32((s32_Max - s32_Min) / 10L);
               break;
            }
         case C_OscNodeDataPoolContent::eSINT64:
            {
               const int64_t s64_Min = orc_Min.GetValueS64();
               const int64_t s64_Max = orc_Max.GetValueS64();
               this->c_ChangeThreshold.SetValueS64((s64_Max - s64_Min) / 10LL);
               break;
            }
         case C_OscNodeDataPoolContent::eFLOAT32:
            {
               const float32_t f32_Min = orc_Min.GetValueF32();
               const float32_t f32_Max = orc_Max.GetValueF32();
               this->c_ChangeThreshold.SetValueF32((f32_Max - f32_Min) / 10.0F);
               break;
            }
         case C_OscNodeDataPoolContent::eFLOAT64:
            {
               const float64_t f64_Min = orc_Min.GetValueF64();
               const float64_t f64_Max = orc_Max.GetValueF64();
               this->c_ChangeThreshold.SetValueF64((f64_Max - f64_Min) / 10.0);
               break;
            }
         default:
            tgl_assert(false);
            break;
         }
      }
      else
      {
         this->c_ChangeThreshold.SetArray(true);
         if (orc_Min.GetArraySize() == orc_Max.GetArraySize())
         {
            this->c_ChangeThreshold.SetArraySize(orc_Min.GetArraySize());
            for (uint32_t u32_ItElement = 0; u32_ItElement < orc_Min.GetArraySize(); ++u32_ItElement)
            {
               //Each type: init default value by 10% of range -> (1/10)
               switch (orc_Min.GetType())
               {
               case C_OscNodeDataPoolContent::eUINT8:
                  {
                     const uint8_t u8_Min = orc_Min.GetValueArrU8Element(u32_ItElement);
                     const uint8_t u8_Max = orc_Max.GetValueArrU8Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrU8Element(
                        static_cast<uint8_t>(u8_Max - u8_Min) / static_cast<uint8_t>(10U),
                        u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eUINT16:
                  {
                     const uint16_t u16_Min = orc_Min.GetValueArrU16Element(u32_ItElement);
                     const uint16_t u16_Max = orc_Max.GetValueArrU16Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrU16Element(static_cast<uint16_t>(u16_Max - u16_Min) /
                                                                   static_cast<uint16_t>(10U),
                                                                   u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eUINT32:
                  {
                     const uint32_t u32_Min = orc_Min.GetValueArrU32Element(u32_ItElement);
                     const uint32_t u32_Max = orc_Max.GetValueArrU32Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrU32Element((u32_Max - u32_Min) / 10UL, u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eUINT64:
                  {
                     const uint64_t u64_Min = orc_Min.GetValueArrU64Element(u32_ItElement);
                     const uint64_t u64_Max = orc_Max.GetValueArrU64Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrU64Element((u64_Max - u64_Min) / 10ULL, u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eSINT8:
                  {
                     const int8_t s8_Min = orc_Min.GetValueArrS8Element(u32_ItElement);
                     const int8_t s8_Max = orc_Max.GetValueArrS8Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrS8Element((s8_Max - s8_Min) / static_cast<int8_t>(10),
                                                                  u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eSINT16:
                  {
                     const int16_t s16_Min = orc_Min.GetValueArrS16Element(u32_ItElement);
                     const int16_t s16_Max = orc_Max.GetValueArrS16Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrS16Element((s16_Max - s16_Min) / static_cast<int16_t>(10),
                                                                   u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eSINT32:
                  {
                     const int32_t s32_Min = orc_Min.GetValueArrS32Element(u32_ItElement);
                     const int32_t s32_Max = orc_Max.GetValueArrS32Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrS32Element((s32_Max - s32_Min) / 10L, u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eSINT64:
                  {
                     const int64_t s64_Min = orc_Min.GetValueArrS64Element(u32_ItElement);
                     const int64_t s64_Max = orc_Max.GetValueArrS64Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrS64Element((s64_Max - s64_Min) / 10LL, u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eFLOAT32:
                  {
                     const float32_t f32_Min = orc_Min.GetValueArrF32Element(u32_ItElement);
                     const float32_t f32_Max = orc_Max.GetValueArrF32Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrF32Element((f32_Max - f32_Min) / 10.0F, u32_ItElement);
                     break;
                  }
               case C_OscNodeDataPoolContent::eFLOAT64:
                  {
                     const float64_t f64_Min = orc_Min.GetValueArrF64Element(u32_ItElement);
                     const float64_t f64_Max = orc_Max.GetValueArrF64Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueArrF64Element((f64_Max - f64_Min) / 10.0, u32_ItElement);
                     break;
                  }
               default:
                  tgl_assert(false);
                  break;
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
