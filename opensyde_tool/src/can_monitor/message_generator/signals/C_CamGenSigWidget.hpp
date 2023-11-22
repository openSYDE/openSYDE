//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Top widget for message generator signal edit (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECAMGENSIGWIDGET_HPP
#define C_OGECAMGENSIGWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamGenSigWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamGenSigWidget(QWidget * const opc_Parent = NULL);
   ~C_CamGenSigWidget(void) override;

   void InitStaticNames(void) const;

   void SaveUserSettings(void) const;
   void LoadUserSettings(void);

   void TriggerSignalReload(void);
   void UpdateMessageDlc(const uint32_t ou32_MessageIndex) const;
   void UpdateSelection(const uint32_t ou32_NumSelectedItems, const uint32_t ou32_Row);
   void UpdateAutoProtocolSupport(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateMessageData(const uint32_t ou32_MessageIndex);
   void SigTriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);

private:
   Ui::C_CamGenSigWidget * mpc_Ui;
   uint32_t mu32_NumSelectedItems;
   uint32_t mu32_Row;

   void m_Reset(void);

   //Avoid call
   C_CamGenSigWidget(const C_CamGenSigWidget &);
   C_CamGenSigWidget & operator =(const C_CamGenSigWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
