//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard label item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDALABELBASE_H
#define C_GISVDALABELBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_SyvDaItDashboardLabelWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaLabelBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaLabelBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                     const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                     QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvDaLabelBase(void) override;

   int32_t type(void) const override;
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                        const bool oq_DarkMode) override;
   void ReInitializeSize(void) override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   void UpdateShowValue(void) override;
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value) override;
   void ConnectionActiveChanged(const bool oq_Active) override;
   bool CallProperties(void) override;

   void UpdateTypePe(const stw::opensyde_gui_logic::C_PuiSvDbLabel::E_Type oe_Type, const bool oq_ShowCaption,
                     const bool oq_ShowUnit);
   void SetCaptionPe(const QString & orc_Value) const;
   void SetValuePe(const QString & orc_Value) const;
   void SetUnitPe(const QString & orc_Value);

private:
   C_SyvDaItDashboardLabelWidget * mpc_LabelWidget;

   void m_UpdateCaption(const stw::opensyde_gui_logic::C_PuiSvDbLabel & orc_Data) const;
   void m_UpdateStaticValue(const stw::opensyde_gui_logic::C_PuiSvDbLabel & orc_Label);

   //Avoid call
   C_GiSvDaLabelBase(const C_GiSvDaLabelBase &);
   C_GiSvDaLabelBase & operator =(const C_GiSvDaLabelBase &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
