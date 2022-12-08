#include "precomp_headers.hpp"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "C_CanMonProtocolTarget.hpp"

#include "stwtypes.hpp"
#include "C_CanMonProtocolCanOpen.hpp"

//---------------------------------------------------------------------------


using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

//---------------------------------------------------------------------------
//have the protocol constants here as copies, so we can put this class into a library of its own
// without having to add all the protocol header files

//CANopen constants
static const uint32_t NMT_ZERO_ID      =    0U;
static const uint32_t SYNC_ID          =  128U;
static const uint32_t EMCY_AREA_LOW    =  129U;
static const uint32_t EMCY_AREA_HIGH   =  256U;
//static const uint32 SDO_TX_AREA_LOW  = 1409U;
//static const uint32 SDO_TX_AREA_HIGH = 1536U;
//static const uint32 SDO_RX_AREA_LOW  = 1537U;
static const uint32_t SDO_RX_AREA_HIGH = 1664U;
static const uint32_t GUARD_AREA_LOW   = 1793U;
static const uint32_t GUARD_AREA_HIGH  = 1920U;
static const uint32_t SERVER_BASE_ID   = 1408U;
static const uint32_t CLIENT_BASE_ID   = 1536U;

static const uint8_t CS_START_NODE           =   1U;
static const uint8_t CS_STOP_NODE            =   2U;
static const uint8_t CS_ENTER_PREOPERATIONAL = 128U;
static const uint8_t CS_RESET_NODE           = 129U;
static const uint8_t CS_RESET_COMM           = 130U;

static const uint8_t CCS_INIT_UPLOAD      = 0x40U;
static const uint8_t SCS_INIT_UPLOAD      = 0x40U;
static const uint8_t CCS_SEGMENT_UPLOAD   = 0x60U;
static const uint8_t SCS_SEGMENT_UPLOAD   = 0x00U;
static const uint8_t CCS_INIT_DOWNLOAD    = 0x20U;
static const uint8_t SCS_INIT_DOWNLOAD    = 0x60U;
static const uint8_t CCS_SEGMENT_DOWNLOAD = 0x00U;
static const uint8_t SCS_SEGMENT_DOWNLOAD = 0x20U;
static const uint8_t ABORT_DOMAIN         = 0x80U;

static const uint32_t SDO_ERROR_TOGGLE_BIT_NOT_ALTERNATED = 0x05030000U;
static const uint32_t SDO_ERROR_TIMEOUT                   = 0x05040000U;
static const uint32_t SDO_ERROR_INVALID_COMMAND_SPCIFIER  = 0x05040001U;
static const uint32_t SDO_ERROR_INVALID_BLOCK_SIZE        = 0x05040002U;
static const uint32_t SDO_ERROR_INVALID_SEQUENCE_NUMBER   = 0x05040003U;
static const uint32_t SDO_ERROR_CRC_ERROR                 = 0x05040004U;
static const uint32_t SDO_ERROR_OUT_OF_MEMORY             = 0x05040005U;
static const uint32_t SDO_ERROR_UNSUPPORTED_ACCESS        = 0x06010000U;
static const uint32_t SDO_ERROR_READING_A_WO_OBJECT       = 0x06010001U;
static const uint32_t SDO_ERROR_WRITING_AN_RO_OBJECT      = 0x06010002U;
static const uint32_t SDO_ERROR_OBJECT_DOES_NOT_EXIST     = 0x06020000U;
static const uint32_t SDO_ERROR_OBJECT_CANNOT_BE_MAPPED   = 0x06020041U;
static const uint32_t SDO_ERROR_MAPPING_OVERFLOW          = 0x06040042U;
static const uint32_t SDO_ERROR_PARAMETER_INCOMPATIBLE    = 0x06040043U;
static const uint32_t SDO_ERROR_INTERNAL_INCOMPATIBILITY  = 0x06040047U;
static const uint32_t SDO_ERROR_HARDWARE_ERROR            = 0x06060000U;
static const uint32_t SDO_ERROR_LENGTH_DOES_NOT_MATCH     = 0x06070010U;
static const uint32_t SDO_ERROR_LENGTH_TOO_HIGH           = 0x06070012U;
static const uint32_t SDO_ERROR_LENGTH_TOO_LOW            = 0x06070013U;
static const uint32_t SDO_ERROR_SUBINDEX_DOES_NOT_EXIST   = 0x06090011U;
static const uint32_t SDO_ERROR_VALUE_RANGE_EXCEEDED      = 0x06090030U;
static const uint32_t SDO_ERROR_VALUE_TOO_HIGH            = 0x06090031U;
static const uint32_t SDO_ERROR_VALUE_TOO_LOW             = 0x06090032U;
static const uint32_t SDO_ERROR_MAX_VALUE_LT_MIN_VALUE    = 0x06090036U;
static const uint32_t SDO_ERROR_GENERAL_ERROR             = 0x08000000U;
static const uint32_t SDO_ERROR_APPLICATION_STORAGE       = 0x08000020U;
static const uint32_t SDO_ERROR_LOCAL_CONTROL             = 0x08000021U;
static const uint32_t SDO_ERROR_DEVICE_STATE              = 0x08000022U;
static const uint32_t SDO_ERROR_DYNAMIC_OD_ERROR          = 0x08000023U;

//-----------------------------------------------------------------------------
/*!
   \brief   Convert CAN message to text representation.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolCanOpen::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text;
   C_SclString c_Help;
   C_SclString c_Help2;
   uint16_t u16_Index;
   uint8_t u16_SubIndex;
   uint32_t u32_Size;
   int32_t j;
   c_Text = "";

   if ((orc_Msg.u32_ID >= GUARD_AREA_LOW) && (orc_Msg.u32_ID < GUARD_AREA_HIGH)) //Nodeguarding / Heartbeat / Bootup
   {
      if ((orc_Msg.u8_DLC >= 1U) && (orc_Msg.u8_RTR == 0U))
      {
         c_Text = "HB/Guard " +
              m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.u32_ID - (GUARD_AREA_LOW - 1))) + " : TGL:"
              + C_SclString::IntToStr(orc_Msg.au8_Data[0] >> 7) + " : ST:";
         switch (orc_Msg.au8_Data[0] & 0x7FU)
         {
         case 0:
            c_Help = "BOOTUP";
            break;
         case 4:
            c_Help = "STOPPED";
            break;
         case 5:
            c_Help = "OPERATIONAL";
            break;
         case 127:
            c_Help = "PRE-OPERATIONAL";
            break;
         default:
            c_Help = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[0] & 0x7FU));
            break;
         }
         c_Text = c_Text + c_Help;
      }
      else //probably a guard request from a master
      {
         c_Text = "Guard RQ " + m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.u32_ID - (GUARD_AREA_LOW - 1)));
      }
   }
   if (orc_Msg.u32_ID == NMT_ZERO_ID)
   {
      if (orc_Msg.au8_Data[1] == 0)
      {
         c_Help = "NMT ALL NODES: ";
      }
      else
      {
         c_Help = "NMT NODE " + m_GetByteAsStringFormat(orc_Msg.au8_Data[1]) + ": ";
      }
      switch (orc_Msg.au8_Data[0])
      {
      case CS_START_NODE          :
         c_Text = c_Help + "START NODE";
         break;
      case CS_STOP_NODE           :
         c_Text = c_Help + "STOP NODE";
         break;
      case CS_ENTER_PREOPERATIONAL:
         c_Text = c_Help + "ENTER PREOPERATIONAL";
         break;
      case CS_RESET_NODE          :
         c_Text = c_Help + "RESET NODE";
         break;
      case CS_RESET_COMM          :
         c_Text = c_Help + "RESET COMMUNICATION";
         break;
      default:
         c_Text = c_Help + "?\?\?";
      }
      if (orc_Msg.u8_DLC != 2)
      {
         c_Text += " (*DLC Error*)";
      }
   }
   if (orc_Msg.u32_ID == SYNC_ID)
   {
      c_Text = "SYNC";
   }
   if ((orc_Msg.u32_ID >= EMCY_AREA_LOW) && (orc_Msg.u32_ID < EMCY_AREA_HIGH))
   {
      c_Text = "EMCY NODE " + m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.u32_ID - (EMCY_AREA_LOW - 1))) + ":";
      if (orc_Msg.u8_DLC == 0)
      {
         c_Text = c_Text + " Startup";
      }
      else
      {
         if (orc_Msg.u8_DLC != 8)
         {
            c_Text = c_Text + " (*DLC Error*)";
         }
         else
         {
            c_Help = "";
            for (j = 0; j < 8; j++)
            {
               c_Help += (" " + m_GetByteAsStringFormat(orc_Msg.au8_Data[j]));
            }
            c_Text += c_Help + " ";
            //default error codes
            switch (orc_Msg.au8_Data[1])
            {
            case 0x00:
               c_Help = "(err reset)";
               break;
            case 0x10:
               c_Help = "(generic err)";
               break;
            case 0x20:
               c_Help = "(current)";
               break;
            case 0x21:
               c_Help = "(current input side)";
               break;
            case 0x22:
               c_Help = "(current inside)";
               break;
            case 0x23:
               c_Help = "(current output side)";
               break;
            case 0x30:
               c_Help = "(voltage)";
               break;
            case 0x31:
               c_Help = "(mains voltage)";
               break;
            case 0x32:
               c_Help = "(voltage inside)";
               break;
            case 0x33:
               c_Help = "(output voltage)";
               break;
            case 0x40:
               c_Help = "(temperature)";
               break;
            case 0x41:
               c_Help = "(ambient temperature)";
               break;
            case 0x42:
               c_Help = "(device temperature)";
               break;
            case 0x50:
               c_Help = "(device hardware)";
               break;
            case 0x60:
               c_Help = "(device software)";
               break;
            case 0x61:
               c_Help = "(internal software)";
               break;
            case 0x62:
               c_Help = "(user software)";
               break;
            case 0x63:
               c_Help = "(data set)";
               break;
            case 0x70:
               c_Help = "(additional modules)";
               break;
            case 0x80:
               c_Help = "(monitoring)";
               break;
            case 0x81:
               c_Help = "(communication)";
               break;
            case 0x82:
               c_Help = "(protocol error)";
               break;
            case 0x90:
               c_Help = "(external error)";
               break;
            case 0xF0:
               c_Help = "(additional functions)";
               break;
            case 0xFF:
               c_Help = "(device specific)";
               break;
            default:
               c_Help = "";
               break;
            }
            c_Text += c_Help;
         }
      }
   }
   if ((orc_Msg.u32_ID > SERVER_BASE_ID) && (orc_Msg.u32_ID < SDO_RX_AREA_HIGH))
   {
      if (orc_Msg.u32_ID < CLIENT_BASE_ID)
      {
         c_Help = "SDO SERVER " + m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.u32_ID - SERVER_BASE_ID))+": ";
         switch (orc_Msg.au8_Data[0] & 0xE0U)
         {
            case SCS_INIT_UPLOAD     :
               u16_Index = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]);
               u16_SubIndex = orc_Msg.au8_Data[3];
               if ((orc_Msg.au8_Data[0] & 0x02U) == 0x02U)
               {
                  //expedited
                  c_Help = c_Help + "INIT UPLOAD EXP RES ";
                  (void)c_Help2.PrintFormatted("%X,%02X  ", u16_Index, u16_SubIndex);
                  if ((orc_Msg.au8_Data[0] & 0x01U) == 0x01U) //size is indicated
                  {
                     u32_Size = 4 - ((orc_Msg.au8_Data[0] & 0x0CU) >> 2);
                     c_Help2 += ("Size:" + m_GetByteAsStringFormat(static_cast<uint8_t>(u32_Size)) + " ");
                  }
                  else
                  {
                     u32_Size = 4;
                     c_Help2 += ("Size:?\?\? ");
                  }
                  c_Help2 += "DB: ";
                  for (j = 0; j < static_cast<int32_t>(u32_Size); j++)
                  {
                     c_Help2 += (m_GetByteAsStringFormat(orc_Msg.au8_Data[j + 4]) +" ");
                  }
                  c_Text = c_Help + c_Help2;
               }
               else
               {
                  //segmented
                  c_Help = c_Help + "INIT UPLOAD SEG RES ";
                  (void)c_Help2.PrintFormatted("%X,%02X  ", u16_Index, u16_SubIndex);
                  u32_Size =  mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]);
                  c_Help2 += ("Size: " + m_GetValueDecHex(u32_Size));
                  c_Text = c_Help + c_Help2;
               }
               break;
            case SCS_SEGMENT_UPLOAD  :
               c_Help = c_Help + "UPLOAD SEG RES ";
               c_Help2 = "";
               if ((orc_Msg.au8_Data[0] & 0x0EU) != 0U) //size specified ?
               {
                  u32_Size = 7 - ((orc_Msg.au8_Data[0] & 0x0EU) >> 1);
                  c_Help2 += ("Size:" + m_GetByteAsStringFormat(static_cast<uint8_t>(u32_Size)) + " ");
               }
               else
               {
                  u32_Size = 7;
                  c_Help2 += ("Size:?\?\?");
               }
               for (j = 0; j < static_cast<int32_t>(u32_Size); j++)
               {
                  c_Help2 += (m_GetByteAsStringFormat(orc_Msg.au8_Data[j + 1]) +" ");
               }
               c_Text = c_Help + c_Help2;
               break;
            case SCS_INIT_DOWNLOAD   :
               c_Help = c_Help + "INIT DOWNLOAD RES ";
               u16_Index = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]);
               u16_SubIndex = orc_Msg.au8_Data[3];
               (void)c_Help2.PrintFormatted("%X,%02X", u16_Index, u16_SubIndex);
               c_Text = c_Help + c_Help2;
               break;
            case SCS_SEGMENT_DOWNLOAD:
               c_Help = c_Help + "DOWNLOAD SEG RES ";
               c_Text = c_Help;
               break;
            case ABORT_DOMAIN        :
               c_Help = c_Help + "ABORT ";
               u16_Index = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]);
               u16_SubIndex = orc_Msg.au8_Data[3];
               (void)c_Help2.PrintFormatted("%X,%02X  Reason:", u16_Index, u16_SubIndex);
               for (j = 0; j < 4; j++)
               {
                  c_Help2 += (m_GetByteAsStringFormat(orc_Msg.au8_Data[j + 4]) + " ");
               }
               c_Text = c_Help + c_Help2;

               //add real error codes:
               c_Help2 = "";
               switch (mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]))
               {
               case SDO_ERROR_TOGGLE_BIT_NOT_ALTERNATED:
                  c_Help2 = "(toggle bit not alternated)";
                  break;
               case SDO_ERROR_TIMEOUT                  :
                  c_Help2 = "(SDO timed out)";
                  break;
               case SDO_ERROR_INVALID_COMMAND_SPCIFIER :
                  c_Help2 = "(command specifier invalid)";
                  break;
               case SDO_ERROR_INVALID_BLOCK_SIZE       :
                  c_Help2 = "(invalid block size)";
                  break;
               case SDO_ERROR_INVALID_SEQUENCE_NUMBER  :
                  c_Help2 = "(invalid sequence number)";
                  break;
               case SDO_ERROR_CRC_ERROR                :
                  c_Help2 = "(CRC error)";
                  break;
               case SDO_ERROR_OUT_OF_MEMORY            :
                  c_Help2 = "(out of memory)";
                  break;
               case SDO_ERROR_UNSUPPORTED_ACCESS       :
                  c_Help2 = "(unsupported access)";
                  break;
               case SDO_ERROR_READING_A_WO_OBJECT      :
                  c_Help2 = "(reading a WO object)";
                  break;
               case SDO_ERROR_WRITING_AN_RO_OBJECT     :
                  c_Help2 = "(writing an RO object)";
                  break;
               case SDO_ERROR_OBJECT_DOES_NOT_EXIST    :
                  c_Help2 = "(object does not exist)";
                  break;
               case SDO_ERROR_OBJECT_CANNOT_BE_MAPPED  :
                  c_Help2 = "(object cannot be mapped)";
                  break;
               case SDO_ERROR_MAPPING_OVERFLOW         :
                  c_Help2 = "(mapping overflow)";
                  break;
               case SDO_ERROR_PARAMETER_INCOMPATIBLE   :
                  c_Help2 = "(parameter incompatible)";
                  break;
               case SDO_ERROR_INTERNAL_INCOMPATIBILITY :
                  c_Help2 = "(internal incompatibility)";
                  break;
               case SDO_ERROR_HARDWARE_ERROR           :
                  c_Help2 = "(hardware error)";
                  break;
               case SDO_ERROR_LENGTH_DOES_NOT_MATCH    :
                  c_Help2 = "(data length does not match)";
                  break;
               case SDO_ERROR_LENGTH_TOO_HIGH          :
                  c_Help2 = "(data length too high)";
                  break;
               case SDO_ERROR_LENGTH_TOO_LOW           :
                  c_Help2 = "(data length too low)";
                  break;
               case SDO_ERROR_SUBINDEX_DOES_NOT_EXIST  :
                  c_Help2 = "(subindex does not exist)";
                  break;
               case SDO_ERROR_VALUE_RANGE_EXCEEDED     :
                  c_Help2 = "(value range exceeded)";
                  break;
               case SDO_ERROR_VALUE_TOO_HIGH           :
                  c_Help2 = "(value too high)";
                  break;
               case SDO_ERROR_VALUE_TOO_LOW            :
                  c_Help2 = "(value too low)";
                  break;
               case SDO_ERROR_MAX_VALUE_LT_MIN_VALUE   :
                  c_Help2 = "(min is greater than max)";
                  break;
               case SDO_ERROR_GENERAL_ERROR            :
                  c_Help2 = "(general error)";
                  break;
               case SDO_ERROR_APPLICATION_STORAGE      :
                  c_Help2 = "(application storage)";
                  break;
               case SDO_ERROR_LOCAL_CONTROL            :
                  c_Help2 = "(local control)";
                  break;
               case SDO_ERROR_DEVICE_STATE             :
                  c_Help2 = "(present device state)";
                  break;
               case SDO_ERROR_DYNAMIC_OD_ERROR         :
                  c_Help2 = "(dynamic OD error)";
                  break;
               default:
                  break;
               }
               if (c_Help2 != "")
               {
                  c_Text = c_Text + " "+c_Help2;
               }
               break;
            default:
               break;
         }
      }
      else
      {
         c_Help = "SDO CLIENT " + m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.u32_ID - CLIENT_BASE_ID)) + ": ";
         switch (orc_Msg.au8_Data[0] & 0xE0U)
         {
         case CCS_INIT_UPLOAD     :
            c_Help = c_Help + "INIT UPLOAD REQ ";
            u16_Index = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]);
            u16_SubIndex = orc_Msg.au8_Data[3];
            (void)c_Help2.PrintFormatted("%X,%02X  ", u16_Index, u16_SubIndex);
            c_Text = c_Help + c_Help2;
            break;
         case CCS_SEGMENT_UPLOAD  :
            c_Text = c_Help + "UPLOAD SEG REQ ";
            break;
         case CCS_INIT_DOWNLOAD   :
            u16_Index = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]);
            u16_SubIndex = orc_Msg.au8_Data[3];
            (void)c_Help2.PrintFormatted("%X,%02X  ", u16_Index, u16_SubIndex);
            if ((orc_Msg.au8_Data[0] & 0x02U) == 0x02U)
            {
               //expedited
               c_Help = c_Help + "INIT DOWNLOAD EXP REQ ";
               if ((orc_Msg.au8_Data[0] & 0x01U) == 0x01U)
               {
                  u32_Size = 4U - ((orc_Msg.au8_Data[0] & 0x0CU) >> 2); //size is indicated
                  c_Help2 += ("Size:" + m_GetByteAsStringFormat(static_cast<uint8_t>(u32_Size)) + " ");
               }
               else
               {
                  u32_Size = 4U;
                  c_Help2 += ("Size:?\?\?");
               }
               for (j = 0; j < static_cast<int32_t>(u32_Size); j++)
               {
                  c_Help2 += (m_GetByteAsStringFormat(orc_Msg.au8_Data[j + 4] ) + " ");
               }
               c_Text = c_Help + c_Help2;
            }
            else
            {
               //segmented
               c_Help = c_Help + "INIT DOWNLOAD SEG REQ ";
               u32_Size = mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]);
               c_Help2 += ("Size: " + m_GetValueDecHex(u32_Size));
               c_Text = c_Help + c_Help2;
            }
            break;
         case CCS_SEGMENT_DOWNLOAD:
            c_Help = c_Help + "DOWNLOAD SEG REQ ";
            c_Help2 = "";
            if ((orc_Msg.au8_Data[0] & 0x0EU) != 0U) //size specified ?
            {
               u32_Size = 7U - ((orc_Msg.au8_Data[0] & 0x0EU) >> 1);
               c_Help2 += ("Size:" + m_GetByteAsStringFormat(static_cast<uint8_t>(u32_Size)) + " ");
            }
            else
            {
               u32_Size = 7U;
               c_Help2 += ("Size:?\?\?");
            }
            for (j = 0; j < static_cast<int32_t>(u32_Size); j++)
            {
               c_Help2 += (m_GetByteAsStringFormat(orc_Msg.au8_Data[j + 1] ) + " ");
            }
            c_Text = c_Help + c_Help2;
            break;
         case ABORT_DOMAIN        :
            c_Help = c_Help + "ABORT ";
            u16_Index = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]);
            u16_SubIndex = orc_Msg.au8_Data[3];
            (void)c_Help2.PrintFormatted("%X,%02X  Reason:", u16_Index, u16_SubIndex);
            for (j = 0; j < 4; j++)
            {
               c_Help2 += (m_GetByteAsStringFormat(orc_Msg.au8_Data[j + 4]) + " ");
            }
            c_Text = c_Help + c_Help2;
            break;
         default:
            break;
         }
      }
   }
   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolCanOpen::GetProtocolName(void) const
{
   return "CANopen";
}

//---------------------------------------------------------------------------

