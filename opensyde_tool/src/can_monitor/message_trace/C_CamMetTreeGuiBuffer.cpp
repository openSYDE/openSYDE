//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Buffer for max performance model additions (implementation)

   Buffer for max performance model additions

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamMetTreeGuiBuffer.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetTreeGuiBuffer::C_CamMetTreeGuiBuffer(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mq_Connected(false)
{
   mc_Timer.setInterval(100);
   mc_Timer.start();
   connect(this, &C_CamMetTreeGuiBuffer::SigInternalTrigger, this, &C_CamMetTreeGuiBuffer::m_HandleUpdateUi);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new data into queue to update ui later

   \param[in] orc_NewData Single, new data entry
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeGuiBuffer::HandleData(const C_CamMetTreeLoggerData & orc_NewData)
{
   this->mc_BufferMutex.lock();
   this->mc_Buffer.push_back(orc_NewData);
   this->mc_BufferMutex.unlock();
   if (this->mq_Connected == false)
   {
      this->mq_Connected = true;
      connect(&mc_Timer, &QTimer::timeout, this, &C_CamMetTreeGuiBuffer::SigInternalTrigger);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes all already added entries without reading
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeGuiBuffer::ClearBuffer(void)
{
   this->mc_BufferMutex.lock();
   this->mc_Buffer.clear();
   this->mc_BufferMutex.unlock();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger UI update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetTreeGuiBuffer::m_HandleUpdateUi(void)
{
   std::list<C_CamMetTreeLoggerData> c_BufferCopy;
   this->mc_BufferMutex.lock();
   c_BufferCopy = this->mc_Buffer;
   this->mc_Buffer.clear();
   this->mc_BufferMutex.unlock();
   if (c_BufferCopy.empty() == false)
   {
      Q_EMIT this->SigUpdateUi(c_BufferCopy);
   }
}
