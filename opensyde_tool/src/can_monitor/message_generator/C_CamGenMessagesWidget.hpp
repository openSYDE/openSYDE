//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for all displaying and interacting with manually configured messages (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENMESSAGESWIDGET_HPP
#define C_CAMGENMESSAGESWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamGenMessagesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenMessagesWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamGenMessagesWidget(QWidget * const opc_Parent = NULL);
   ~C_CamGenMessagesWidget(void) override;

   void InitStaticNames(void) const;
   void SaveUserSettings(void) const;
   void LoadUserSettings(void) const;
   void RemoveMessagesForFile(const QString & orc_File, const std::vector<uint32_t> * const opc_Indices) const;
   void SetCommunicationStarted(const bool oq_Online);
   bool CheckAndHandleKey(const QString & orc_Input) const;
   void UpdateMessageData(const uint32_t ou32_MessageIndex) const;
   void TriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   void SigRemoveAllCyclicMessages (void);
   void SigSendMessage(const uint32_t ou32_MessageIndex, const uint32_t ou32_TimeToSend);
   void SigUpdateMessageDlc(const uint32_t ou32_MessageIndex);
   void SigSelected(const uint32_t ou32_NumSelectedItems, const uint32_t ou32_Row);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_CamGenMessagesWidget * mpc_Ui;

   void m_InitButtons(void) const;
   void m_UpdateHeading(void) const;
   void m_LoadConfig(void) const;
   void m_OnItemCountChanged(const uint32_t ou32_NewItemCount) const;
   void m_OnTransmissionToggled(const bool oq_Active);

   //Avoid call
   C_CamGenMessagesWidget(const C_CamGenMessagesWidget &);
   C_CamGenMessagesWidget & operator =(const C_CamGenMessagesWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
