//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard widget properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPELABEL_H
#define C_SYVDAPELABEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_SyvDaPeBase.h"
#include "C_PuiSvDbLabel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeLabel;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeLabel :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeLabel(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeLabel(void);

   void InitStaticNames(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbLabel::E_Type GetType(void) const;
   bool GetShowUnit(void) const;
   bool GetShowCaption(void) const;
   QString GetCaption(void) const;
   void SetType(const stw_opensyde_gui_logic::C_PuiSvDbLabel::E_Type oe_Type) const;
   void SetShowUnit(const bool oq_Value) const;
   void SetShowCaption(const bool oq_Value) const;
   void SetCaption(const QString & orc_Value) const;

private:
   //Avoid call
   C_SyvDaPeLabel(const C_SyvDaPeLabel &);
   C_SyvDaPeLabel & operator =(const C_SyvDaPeLabel &);

   void m_UpdatePreview(void);
   void m_ToUpper(void) const;

   Ui::C_SyvDaPeLabel * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE_DEFAULT;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE_TRANSPARENT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
