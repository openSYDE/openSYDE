//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Artificial data generator (implementation)

   Artificial data generator

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cstdlib>

#include "CSCLString.h"
#include "C_CamMetTreeDataSource.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_scl;
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     29.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamMetTreeDataSource::C_CamMetTreeDataSource(QObject * const opc_Parent) :
   QObject(opc_Parent)
{
   this->mc_Timer.setInterval(1);
   this->mc_Timer.start();
   connect(this, &C_CamMetTreeDataSource::SigInternalTrigger, this, &C_CamMetTreeDataSource::m_GenerateNewData);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start sending artificial data

   \created     29.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamMetTreeDataSource::Start(void)
{
   connect(&mc_Timer, &QTimer::timeout, this, &C_CamMetTreeDataSource::SigInternalTrigger);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Internal generator for new data

   \created     29.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamMetTreeDataSource::m_GenerateNewData(void)
{
   C_CamMetTreeLoggerData c_NewData;

   c_NewData.c_CanDataDec = C_SCLString::IntToStr(std::rand());
   c_NewData.c_CanDataHex = C_SCLString::IntToStr(std::rand());
   c_NewData.c_CanDlc = C_SCLString::IntToStr(std::rand());
   c_NewData.c_CanIdDec = C_SCLString::IntToStr(std::rand());
   c_NewData.c_CanIdHex = C_SCLString::IntToStr(std::rand());
   c_NewData.c_ProtocolTextDec = C_SCLString::IntToStr(std::rand());
   c_NewData.c_ProtocolTextHex = C_SCLString::IntToStr(std::rand());
   c_NewData.c_TimeStampAbsolute = C_SCLString::IntToStr(std::rand());
   c_NewData.c_TimeStampRelative = C_SCLString::IntToStr(std::rand());

   Q_EMIT this->SigNewData(c_NewData);
}
