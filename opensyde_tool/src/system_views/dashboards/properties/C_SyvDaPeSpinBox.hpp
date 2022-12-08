//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard spin box properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPESPINBOX_H
#define C_SYVDAPESPINBOX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_SyvDaPeBase.hpp"

#include "C_PuiSvDbSpinBox.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeSpinBox;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeSpinBox :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeSpinBox(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeSpinBox(void) override;

   void InitStaticNames(void) const;

   bool GetShowUnit(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbSpinBox::E_Type GetType(void) const;
   void SetShowUnit(const bool oq_Value) const;
   void SetType(const stw::opensyde_gui_logic::C_PuiSvDbSpinBox::E_Type oe_Type) const;

private:
   //Avoid call
   C_SyvDaPeSpinBox(const C_SyvDaPeSpinBox &);
   C_SyvDaPeSpinBox & operator =(const C_SyvDaPeSpinBox &) &;

   void m_UpdatePreview(void);

   Ui::C_SyvDaPeSpinBox * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;

   static const int32_t mhs32_INDEX_STYLE_TYPE1;
   static const int32_t mhs32_INDEX_STYLE_TYPE2;
};
}
}

#endif
