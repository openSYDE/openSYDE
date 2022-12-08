//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILILABEL_H
#define C_GILILABEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QFont>
#include <QSvgRenderer>

#include "stwtypes.hpp"

#include "C_GiBiRectBaseGroup.hpp"
#include "C_GiText.hpp"
#include "C_PuiBsTextElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiTextElement :
   public C_GiBiRectBaseGroup
{
   Q_OBJECT

public:
   C_GiBiTextElement(const uint64_t & oru64_Id, const bool oq_Editable, QGraphicsItem * const opc_Parent = NULL);
   C_GiBiTextElement(const uint64_t & oru64_Id = 0, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiBiTextElement() override;

   int32_t type() const override;
   bool OpenStyleDialog(const bool oq_DarkMode);
   void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;

   void RestoreDefaultCursor(void) override;
   void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;

   void SetText(const QString & orc_Text);
   void SetFontStyle(const QFont & orc_Font);
   virtual void SetFontColor(const QColor & orc_Color);

   QString GetText(void) const;
   QFont GetFontStyle(void) const;
   QColor GetFontColor(void) const;
   void ApplyStyle(const QFont & orc_Font, const QColor & orc_Color);

   void SetEditable(const bool oq_Editable);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigTextInteractionModeStateChanged(const bool & orq_On);

protected:
   void m_LoadTextElementData(const stw::opensyde_gui_logic::C_PuiBsTextElement * const opc_Data,
                              const bool oq_DarkMode);
   void m_UpdateTextElementData(stw::opensyde_gui_logic::C_PuiBsTextElement * const opc_Data,
                                const bool oq_DarkMode) const;

   void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) override;

   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   bool sceneEventFilter(QGraphicsItem * const opc_Watched, QEvent * const opc_Event) override;

   C_GiText * mpc_TextItem;

private:
   //Avoid call
   C_GiBiTextElement(const C_GiBiTextElement &);
   C_GiBiTextElement & operator =(const C_GiBiTextElement &) &; //lint !e1511 //we want to hide the base func.

   void m_Init(void);
   void m_HandleTextInteractionMode(const bool & orq_On);

   bool mq_Editable;

   static const float64_t mhf64_MIN_WIDTH_TEXT_ELEMENT;
   static const float64_t mhf64_MIN_HEIGHT_TEXT_ELEMENT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
