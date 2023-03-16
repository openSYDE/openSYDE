//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic heading widget for subsections of settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEWISETTINGSUBSECTION_H
#define C_CAMOGEWISETTINGSUBSECTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamOgeWiSettingSubSection;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeWiSettingSubSection :
   public C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamOgeWiSettingSubSection(QWidget * const opc_Parent = NULL);
   virtual ~C_CamOgeWiSettingSubSection(void);

   void SetTitle(const QString & orc_Title) const;
   void SetIcon(const QString & orc_Icon) const;
   void SetToggle(const bool oq_HasToggle);
   void SetToggleToolTip(const QString & orc_Heading, const QString & orc_Content) const;
   void SetToggleState(const bool oq_Enabled) const;
   void SetExpanded(const bool oq_Expanded) const;
   void ShowExpandButton(const bool oq_Visible) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigExpandSection(const bool oq_Expand); // true: expand section, false: collapse section
   void SigToggled(const bool oq_On);
   void SigHide(void);

protected:
   void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   Ui::C_CamOgeWiSettingSubSection * mpc_Ui;

   //Avoid call
   C_CamOgeWiSettingSubSection(const C_CamOgeWiSettingSubSection &);
   C_CamOgeWiSettingSubSection & operator =(const C_CamOgeWiSettingSubSection &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
