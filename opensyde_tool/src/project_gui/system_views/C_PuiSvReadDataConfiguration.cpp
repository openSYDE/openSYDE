//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for read data element configuration information (implementation)

   Data class for read data element configuration information

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_PuiSvReadDataConfiguration.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
void C_PuiSvReadDataConfiguration::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_TransmissionMode, sizeof(this->e_TransmissionMode), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_RailIndex, sizeof(this->u8_RailIndex), oru32_HashValue);

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
sint32 C_PuiSvReadDataConfiguration::InitDefaultThreshold(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max)
{
   sint32 s32_Retval = C_NO_ERR;

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
         case C_OSCNodeDataPoolContent::eUINT8:
            {
               const uint8 u8_Min = orc_Min.GetValueU8();
               const uint8 u8_Max = orc_Max.GetValueU8();
               this->c_ChangeThreshold.SetValueU8((u8_Max - u8_Min) / static_cast<uint8>(10U));
               break;
            }
         case C_OSCNodeDataPoolContent::eUINT16:
            {
               const uint16 u16_Min = orc_Min.GetValueU16();
               const uint16 u16_Max = orc_Max.GetValueU16();
               this->c_ChangeThreshold.SetValueU16((u16_Max - u16_Min) / static_cast<uint16>(10U));
               break;
            }
         case C_OSCNodeDataPoolContent::eUINT32:
            {
               const uint32 u32_Min = orc_Min.GetValueU32();
               const uint32 u32_Max = orc_Max.GetValueU32();
               this->c_ChangeThreshold.SetValueU32((u32_Max - u32_Min) / 10UL);
               break;
            }
         case C_OSCNodeDataPoolContent::eUINT64:
            {
               const uint64 u64_Min = orc_Min.GetValueU64();
               const uint64 u64_Max = orc_Max.GetValueU64();
               this->c_ChangeThreshold.SetValueU64((u64_Max - u64_Min) / 10ULL);
               break;
            }
         case C_OSCNodeDataPoolContent::eSINT8:
            {
               const sint8 s8_Min = orc_Min.GetValueS8();
               const sint8 s8_Max = orc_Max.GetValueS8();
               this->c_ChangeThreshold.SetValueS8((s8_Max - s8_Min) / static_cast<sint8>(10));
               break;
            }
         case C_OSCNodeDataPoolContent::eSINT16:
            {
               const sint16 s16_Min = orc_Min.GetValueS16();
               const sint16 s16_Max = orc_Max.GetValueS16();
               this->c_ChangeThreshold.SetValueS16((s16_Max - s16_Min) / static_cast<sint16>(10));
               break;
            }
         case C_OSCNodeDataPoolContent::eSINT32:
            {
               const sint32 s32_Min = orc_Min.GetValueS32();
               const sint32 s32_Max = orc_Max.GetValueS32();
               this->c_ChangeThreshold.SetValueS32((s32_Max - s32_Min) / 10L);
               break;
            }
         case C_OSCNodeDataPoolContent::eSINT64:
            {
               const sint64 s64_Min = orc_Min.GetValueS64();
               const sint64 s64_Max = orc_Max.GetValueS64();
               this->c_ChangeThreshold.SetValueS64((s64_Max - s64_Min) / 10LL);
               break;
            }
         case C_OSCNodeDataPoolContent::eFLOAT32:
            {
               const float32 f32_Min = orc_Min.GetValueF32();
               const float32 f32_Max = orc_Max.GetValueF32();
               this->c_ChangeThreshold.SetValueF32((f32_Max - f32_Min) / 10.0F);
               break;
            }
         case C_OSCNodeDataPoolContent::eFLOAT64:
            {
               const float64 f64_Min = orc_Min.GetValueF64();
               const float64 f64_Max = orc_Max.GetValueF64();
               this->c_ChangeThreshold.SetValueF64((f64_Max - f64_Min) / 10.0);
               break;
            }
         }
      }
      else
      {
         this->c_ChangeThreshold.SetArray(true);
         if (orc_Min.GetArraySize() == orc_Max.GetArraySize())
         {
            this->c_ChangeThreshold.SetArraySize(orc_Min.GetArraySize());
            for (uint32 u32_ItElement = 0; u32_ItElement < orc_Min.GetArraySize(); ++u32_ItElement)
            {
               //Each type: init default value by 10% of range -> (1/10)
               switch (orc_Min.GetType())
               {
               case C_OSCNodeDataPoolContent::eUINT8:
                  {
                     const uint8 u8_Min = orc_Min.GetValueAU8Element(u32_ItElement);
                     const uint8 u8_Max = orc_Max.GetValueAU8Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAU8Element((u8_Max - u8_Min) / static_cast<uint8>(10U),
                                                                u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eUINT16:
                  {
                     const uint16 u16_Min = orc_Min.GetValueAU16Element(u32_ItElement);
                     const uint16 u16_Max = orc_Max.GetValueAU16Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAU16Element((u16_Max - u16_Min) / static_cast<uint16>(10U),
                                                                 u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eUINT32:
                  {
                     const uint32 u32_Min = orc_Min.GetValueAU32Element(u32_ItElement);
                     const uint32 u32_Max = orc_Max.GetValueAU32Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAU32Element((u32_Max - u32_Min) / 10UL, u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eUINT64:
                  {
                     const uint64 u64_Min = orc_Min.GetValueAU64Element(u32_ItElement);
                     const uint64 u64_Max = orc_Max.GetValueAU64Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAU64Element((u64_Max - u64_Min) / 10ULL, u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eSINT8:
                  {
                     const sint8 s8_Min = orc_Min.GetValueAS8Element(u32_ItElement);
                     const sint8 s8_Max = orc_Max.GetValueAS8Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAS8Element((s8_Max - s8_Min) / static_cast<sint8>(10),
                                                                u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eSINT16:
                  {
                     const sint16 s16_Min = orc_Min.GetValueAS16Element(u32_ItElement);
                     const sint16 s16_Max = orc_Max.GetValueAS16Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAS16Element((s16_Max - s16_Min) / static_cast<sint16>(10),
                                                                 u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eSINT32:
                  {
                     const sint32 s32_Min = orc_Min.GetValueAS32Element(u32_ItElement);
                     const sint32 s32_Max = orc_Max.GetValueAS32Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAS32Element((s32_Max - s32_Min) / 10L, u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eSINT64:
                  {
                     const sint64 s64_Min = orc_Min.GetValueAS64Element(u32_ItElement);
                     const sint64 s64_Max = orc_Max.GetValueAS64Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAS64Element((s64_Max - s64_Min) / 10LL, u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eFLOAT32:
                  {
                     const float32 f32_Min = orc_Min.GetValueAF32Element(u32_ItElement);
                     const float32 f32_Max = orc_Max.GetValueAF32Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAF32Element((f32_Max - f32_Min) / 10.0F, u32_ItElement);
                     break;
                  }
               case C_OSCNodeDataPoolContent::eFLOAT64:
                  {
                     const float64 f64_Min = orc_Min.GetValueAF64Element(u32_ItElement);
                     const float64 f64_Max = orc_Max.GetValueAF64Element(u32_ItElement);
                     this->c_ChangeThreshold.SetValueAF64Element((f64_Max - f64_Min) / 10.0, u32_ItElement);
                     break;
                  }
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
