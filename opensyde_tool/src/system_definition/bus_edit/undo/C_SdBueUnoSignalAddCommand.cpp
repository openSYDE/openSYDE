//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Signal add undo command (implementation)

   Signal add undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_SdBueUnoSignalAddCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     oru32_SignalIndex      Signal index
   \param[in]     ou16_StartBit          Start bit of signal
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
   \param[in,out] opc_Parent             Optional pointer to parent

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoSignalAddCommand::C_SdBueUnoSignalAddCommand(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                       const uint32 & oru32_SignalIndex, const uint16 ou16_StartBit,
                                                       C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                                                       C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
                                                       QUndoCommand * const opc_Parent) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_MessageId, oru32_SignalIndex, ou16_StartBit, opc_MessageSyncManager,
                                        opc_MessageTreeWidget,
                                        "Add Signal",
                                        opc_Parent)
{
   //Adapt default signal
   if ((((this->mc_OSCSignalCommon.GetArray() == false) &&
         (this->mc_OSCSignalCommon.GetType() == C_OSCNodeDataPoolContent::eUINT8)) &&
        (this->mc_UISignalCommon.q_AutoMinMaxActive == true)) &&
       (this->mc_Signal.u16_ComBitLength == 1U))
   {
      this->mc_OSCSignalCommon.c_MaxValue.SetValueU8(1);
   }
   else
   {
      //Unexpected default values
      tgl_assert(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set initial data

   \param[in] orc_Signal          Signal data (osc)
   \param[in] orc_OSCSignalCommon Signal data (osc common)
   \param[in] orc_UISignalCommon  Signal data (ui common)
   \param[in] orc_UISignal        Signal data (ui)

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::SetInitialData(const C_OSCCanSignal & orc_Signal,
                                                const C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                                const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                                const C_PuiSdNodeCanSignal & orc_UISignal)
{
   this->mc_Signal = orc_Signal;
   this->mc_OSCSignalCommon = orc_OSCSignalCommon;
   this->mc_UISignalCommon = orc_UISignalCommon;
   this->mc_UISignal = orc_UISignal;
   this->mc_UISignal.u8_ColorIndex = 0U;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo add

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::redo(void)
{
   this->Add();
   C_SdBueUnoSignalAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo add

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::undo(void)
{
   C_SdBueUnoSignalAddDeleteBaseCommand::undo();
   this->Delete();
}
