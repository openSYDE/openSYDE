//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard slider properties (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.09.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAPESLIDER_H
#define C_SYVDAPESLIDER_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

#include "C_SyvDaPeBase.h"

#include "C_PuiSvDbSlider.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvDaPeSlider;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SyvDaPeSlider :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeSlider(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPeSlider(void);

   void InitStaticNames(void) const;

   bool GetShowMinMax(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type GetType(void) const;
   void SetShowMinMax(const bool oq_Value) const;
   void SetType(const stw_opensyde_gui_logic::C_PuiSvDbSlider::E_Type oe_Type) const;

private:
   //Avoid call
   C_SyvDaPeSlider(const C_SyvDaPeSlider &);
   C_SyvDaPeSlider & operator =(const C_SyvDaPeSlider &);

   void m_UpdatePreview(void);

   Ui::C_SyvDaPeSlider * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;

   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE1;
   static const stw_types::sintn mhsn_INDEX_STYLE_TYPE2;
};
}

#endif
