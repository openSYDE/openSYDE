//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard toggle item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDATOGGLEBASE_H
#define C_GISVDATOGGLEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubDashboard.h"
#include "C_GiSvDaRectBaseGroup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaToggleBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaToggleBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                      const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                      QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaToggleBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode) override;
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   virtual void SendCurrentValue(void);
   virtual bool CallProperties(void) override;
   virtual void ConnectionActiveChanged(const bool oq_Active);

   void UpdateType(const stw_opensyde_gui_logic::C_PuiSvDbToggle::E_Type oe_Type);

protected:
   virtual bool m_CheckHasValidElements(QString & orc_FirstInvalidElementName) const;

private:
   stw_opensyde_gui_elements::C_OgePubDashboard * mpc_CheckBoxWidget;

   //Avoid call
   C_GiSvDaToggleBase(const C_GiSvDaToggleBase &);
   C_GiSvDaToggleBase & operator =(const C_GiSvDaToggleBase &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
