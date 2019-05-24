//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard progress bar properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEPROGRESSBAR_H
#define C_SYVDAPEPROGRESSBAR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_SyvDaPeBase.h"

#include "C_PuiSvDbProgressBar.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaPeProgressBar;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeProgressBar :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeProgressBar(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeProgressBar(void);

   void InitStaticNames(void) const;

   bool GetShowMinMax(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type GetType(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment GetAlignment(void) const;
   void SetShowMinMax(const bool oq_Value) const;
   void SetType(const stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Type oe_Type);
   void SetAlignment(const stw_opensyde_gui_logic::C_PuiSvDbProgressBar::E_Alignment oe_Alignment) const;

private:
   //Avoid call
   C_SyvDaPeProgressBar(const C_SyvDaPeProgressBar &);
   C_SyvDaPeProgressBar & operator =(const C_SyvDaPeProgressBar &);

   void m_TypeChanged(void);
   void m_FillAlignmentComboBox(const stw_types::sintn osn_Type) const;
   void m_UpdatePreview(void);

   Ui::C_SyvDaPeProgressBar * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;

   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE1;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE2;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE3;

   static const stw_types::sintn mhsn_INDEX_ALIGNMENT_LEFT;
   static const stw_types::sintn mhsn_INDEX_ALIGNMENT_RIGHT;
   static const stw_types::sintn mhsn_INDEX_ALIGNMENT_TOP;
   static const stw_types::sintn mhsn_INDEX_ALIGNMENT_BOTTOM;
};
}

#endif
