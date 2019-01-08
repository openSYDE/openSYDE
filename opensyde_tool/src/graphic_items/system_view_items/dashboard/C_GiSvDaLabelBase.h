//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard label item (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISVDALABELBASE_H
#define C_GISVDALABELBASE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiSvDaRectBaseGroup.h"
#include "C_SyvDaItDashboardLabelWidget.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvDaLabelBase :
   public C_GiSvDaRectBaseGroup
{
public:
   C_GiSvDaLabelBase(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                     const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                     QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvDaLabelBase(void);

   virtual stw_types::sintn type(void) const override;
   virtual void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                const bool oq_DarkMode);
   virtual void ReInitializeSize(void) override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   virtual void UpdateShowValue(void) override;
   virtual void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex,
                                   const stw_types::sintn osn_Value) override;
   virtual void ConnectionActiveChanged(const bool oq_Active) override;
   virtual bool CallProperties(void) override;

   void UpdateTypePe(const stw_opensyde_gui_logic::C_PuiSvDbLabel::E_Type oe_Type, const bool oq_ShowCaption,
                     const bool oq_ShowUnit);
   void SetCaptionPe(const QString & orc_Value) const;
   void SetValuePe(const QString & orc_Value) const;
   void SetUnitPe(const QString & orc_Value);

private:
   C_SyvDaItDashboardLabelWidget * mpc_LabelWidget;

   void m_UpdateCaption(const stw_opensyde_gui_logic::C_PuiSvDbLabel & orc_Data) const;
   void m_UpdateStaticValue(const stw_opensyde_gui_logic::C_PuiSvDbLabel & orc_Label);

   //Avoid call
   C_GiSvDaLabelBase(const C_GiSvDaLabelBase &);
   C_GiSvDaLabelBase & operator =(const C_GiSvDaLabelBase &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
