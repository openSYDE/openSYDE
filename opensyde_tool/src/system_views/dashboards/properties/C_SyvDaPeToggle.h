//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard toggle properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPETOGGLE_H
#define C_SYVDAPETOGGLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_SyvDaPeBase.h"

#include "C_PuiSvDbToggle.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeToggle;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeToggle :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeToggle(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeToggle(void);

   void InitStaticNames(void) const;

   stw_opensyde_gui_logic::C_PuiSvDbToggle::E_Type GetType(void) const;
   void SetType(const stw_opensyde_gui_logic::C_PuiSvDbToggle::E_Type oe_Type) const;

private:
   //Avoid call
   C_SyvDaPeToggle(const C_SyvDaPeToggle &);
   C_SyvDaPeToggle & operator =(const C_SyvDaPeToggle &);

   void m_UpdatePreview(void);

   Ui::C_SyvDaPeToggle * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;

   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE1;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE2;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE3;
};
}

#endif
