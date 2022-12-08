//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard slider properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPESLIDER_H
#define C_SYVDAPESLIDER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_SyvDaPeBase.hpp"

#include "C_PuiSvDbSlider.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaPeSlider;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeSlider :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeSlider(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeSlider(void) override;

   void InitStaticNames(void) const;

   bool GetShowMinMax(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbSlider::E_Type GetType(void) const;
   void SetShowMinMax(const bool oq_Value) const;
   void SetType(const stw::opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type) const;

private:
   //Avoid call
   C_SyvDaPeSlider(const C_SyvDaPeSlider &);
   C_SyvDaPeSlider & operator =(const C_SyvDaPeSlider &) &;

   void m_UpdatePreview(void);

   Ui::C_SyvDaPeSlider * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;

   static const int32_t mhs32_INDEX_STYLE_TYPE1;
   static const int32_t mhs32_INDEX_STYLE_TYPE2;
};
}
}

#endif
