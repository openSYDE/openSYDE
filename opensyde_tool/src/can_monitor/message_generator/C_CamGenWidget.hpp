//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuring and showing the CAN message generation
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENWIDGET_H
#define C_CAMGENWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamGenWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamGenWidget(QWidget * const opc_Parent = NULL);
   ~C_CamGenWidget(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   void TriggerSignalReload(void) const;
   void RemoveMessagesForFile(const QString & orc_File, const std::vector<uint32_t> * const opc_Indices = NULL) const;
   void SetCommunicationStarted(const bool oq_Online) const;
   bool CheckAndHandleKey(const QString & orc_Input) const;

   void ExpandMessageGen(const bool oq_Expand) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigExpandMessageGen(const bool oq_Expand); // true: expand message generator, false: minimize message generator
   void SigRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   void SigRemoveAllCyclicMessages(void);
   void SigSendMessage(const uint32_t ou32_MessageIndex, const uint32_t ou32_TimeToSend);

private:
   Ui::C_CamGenWidget * mpc_Ui;

   void m_OnExpandMessageGen(const bool oq_Expand);

   //Avoid call
   C_CamGenWidget(const C_CamGenWidget &);
   C_CamGenWidget & operator =(const C_CamGenWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
