//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for bus specific lines (header)

   Base class for busses (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.08.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILIBUS_H
#define C_GILIBUS_H

/* -- Includes ------------------------------------------------------------- */

#include <QString>

#include "stwtypes.h"

#include "C_PuiSdDataElement.h"
#include "C_GiBiCustomToolTip.h"
#include "C_GiLiLineGroup.h"
#include "C_GiUnique.h"
#include "C_OSCSystemBus.h"
#include "C_GiTextElementBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiLiBus :
   public C_GiLiLineGroup,
   public stw_opensyde_gui_logic::C_PuiSdDataElement,
   public C_GiBiCustomToolTip,
   public stw_opensyde_gui_logic::C_GiUnique
{
   Q_OBJECT

public:
   C_GiLiBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
             C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
             const std::vector<QPointF> * const opc_Points = NULL, const bool & orq_MiddleLine = false,
             QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiBus();

   virtual stw_types::sintn type() const override;

   virtual bool OpenStyleDialog(void) = 0;
   QString GetName(void) const;
   QString GetBitrate(const bool oq_WithComma) const;

   virtual void LoadData(void) override;
   virtual void UpdateData(void);
   virtual void DeleteData(void);
   virtual stw_opensyde_core::C_OSCSystemBus::E_Type GetType() const = 0;
   virtual void GenerateHint(void);

   virtual void SetDisabledLook(const bool oq_Disabled) override;
   void ApplyStyle(const QColor & orc_LineColor, const stw_types::sintn osn_Width);
   virtual void CheckBusForChanges(void);
   virtual void SetResizing(const bool & orq_ResizeActive) override;
   C_GiTextElementBus * GetTextElementBus(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   //lint -restore

   C_GiTextElementBus * mpc_TextElementName;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideToolTip(void);

private:
   //Avoid call
   C_GiLiBus(const C_GiLiBus &);
   C_GiLiBus & operator =(const C_GiLiBus &);

   void m_BusWasMoved(const stw_types::sint32 & ors32_LineIndex, const QPointF & orc_PositionDifference);
   void m_UpdateTextElementName(void);
   void m_UnregisterTextElement(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
