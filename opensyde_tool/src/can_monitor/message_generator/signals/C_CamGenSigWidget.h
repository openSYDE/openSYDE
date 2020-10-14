//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Top widget for message generator signal edit (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECAMGENSIGWIDGET_H
#define C_OGECAMGENSIGWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamGenSigWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamGenSigWidget(QWidget * const opc_Parent = NULL);
   ~C_CamGenSigWidget(void);

   void InitStaticNames(void) const;

   void SaveUserSettings(void) const;
   void LoadUserSettings(void);

   void TriggerSignalReload(void);
   void UpdateMessageDLC(const stw_types::uint32 ou32_MessageIndex) const;
   void UpdateSelection(const stw_types::uint32 ou32_NumSelectedItems, const stw_types::uint32 ou32_Row);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateMessageData(const stw_types::uint32 ou32_MessageIndex);
   void SigTriggerModelUpdateCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);

private:
   Ui::C_CamGenSigWidget * mpc_Ui;
   stw_types::uint32 mu32_NumSelectedItems;
   stw_types::uint32 mu32_Row;

   void m_Reset(void);

   //Avoid call
   C_CamGenSigWidget(const C_CamGenSigWidget &);
   C_CamGenSigWidget & operator =(const C_CamGenSigWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
