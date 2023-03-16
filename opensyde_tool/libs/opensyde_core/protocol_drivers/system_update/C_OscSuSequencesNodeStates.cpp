//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node state for C_OscSuSequences and its sequences

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscSuSequencesNodeStates.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

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
C_OscSuSequencesNodeFileStates::C_OscSuSequencesNodeFileStates(void) :
   c_FileName(""),
   e_FileExists(eSUSEQ_STATE_NOT_EXECUTED),
   e_FileLoaded(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodeHexFileStates::C_OscSuSequencesNodeHexFileStates(void) :
   C_OscSuSequencesNodeFileStates(),
   e_SignatureBlockAddressFromFileRead(eSUSEQ_STATE_NOT_EXECUTED),
   e_NodeNameCompared(eSUSEQ_STATE_NOT_EXECUTED),
   e_DataDumpFromFileRead(eSUSEQ_STATE_NOT_EXECUTED),
   e_AvailableFlashMemoryChecked(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestDownloadSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_AllTransferDataSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestTransferAddressExitSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodeOtherFileStates::C_OscSuSequencesNodeOtherFileStates(void) :
   C_OscSuSequencesNodeFileStates(),
   e_RequestFileTransferSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_AllTransferDataSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestTransferFileExitSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestTransferFileExitResultSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodePsiFileStates::C_OscSuSequencesNodePsiFileStates(void) :
   C_OscSuSequencesNodeFileStates(),
   e_PsiFileWrote(eSUSEQ_STATE_NOT_EXECUTED)
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodePemFileStates::C_OscSuSequencesNodePemFileStates(void) :
   C_OscSuSequencesNodeFileStates(),
   e_PemFileExtracted(eSUSEQ_STATE_NOT_EXECUTED),
   e_SecurityKeySent(eSUSEQ_STATE_NOT_EXECUTED),
   e_SecurityFlagSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_DebuggerFlagSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodeStwFlHexFileStates::C_OscSuSequencesNodeStwFlHexFileStates(void) :
   C_OscSuSequencesNodeFileStates(),
   e_FileSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodeStates::C_OscSuSequencesNodeStates(void) :
   q_Timeout(false),
   q_AuthenticationError(false),
   q_CouldNotReachedDueToRoutingError(false),
   q_RoutingNodeError(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodeConnectStates::C_OscSuSequencesNodeConnectStates(void) :
   C_OscSuSequencesNodeStates(),
   e_InformationRead(eSUSEQ_STATE_NOT_EXECUTED),
   q_AuthenticationNecessarySet(false),
   q_AuthenticationNecessary(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequencesNodeUpdateStates::C_OscSuSequencesNodeUpdateStates(void) :
   C_OscSuSequencesNodeStates()
{
}
