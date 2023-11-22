//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard toggle item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDATOGGLEBASE_HPP
#define C_GISVDATOGGLEBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubDashboard.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaToggleBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaToggleBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                      const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                      QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaToggleBase(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;
   void UpdateShowValue(void) override;

   void SendCurrentValue(void) override;
   bool CallProperties(void) override;
   void ConnectionActiveChanged(const bool oq_Active) override;

   void UpdateType(const stw::opensyde_gui_logic::C_PuiSvDbToggle::E_Type oe_Type);

protected:
   bool m_CheckHasValidElements(QString & orc_FirstInvalidElementName) const override;

private:
   stw::opensyde_gui_elements::C_OgePubDashboard * mpc_CheckBoxWidget;

   //Avoid call
   C_GiSvDaToggleBase(const C_GiSvDaToggleBase &);
   C_GiSvDaToggleBase & operator =(const C_GiSvDaToggleBase &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
