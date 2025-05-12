//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard widget properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPELABEL_HPP
#define C_SYVDAPELABEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_SyvDaPeBase.hpp"
#include "C_PuiSvDbLabel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeLabel;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeLabel :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeLabel(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeLabel(void) override;

   void InitStaticNames(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbLabel::E_Type GetType(void) const;
   bool GetShowUnit(void) const;
   bool GetShowCaption(void) const;
   QString GetCaption(void) const;
   void SetType(const stw::opensyde_gui_logic::C_PuiSvDbLabel::E_Type oe_Type) const;
   void SetShowUnit(const bool oq_Value) const;
   void SetShowCaption(const bool oq_Value) const;
   void SetCaption(const QString & orc_Value) const;

private:
   //Avoid call
   C_SyvDaPeLabel(const C_SyvDaPeLabel &);
   C_SyvDaPeLabel & operator =(const C_SyvDaPeLabel &) &;

   void m_UpdatePreview(void);

   Ui::C_SyvDaPeLabel * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;
   static const int32_t mhs32_INDEX_STYLE_TYPE_DEFAULT;
   static const int32_t mhs32_INDEX_STYLE_TYPE_TRANSPARENT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
