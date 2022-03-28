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

#include "stwtypes.h"

#include "C_GiBiRectBaseGroup.h"
#include "C_GiText.h"
#include "C_PuiBsTextElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiTextElement :
   public C_GiBiRectBaseGroup
{
   Q_OBJECT

public:
   C_GiBiTextElement(const stw_types::uint64 & oru64_ID, const bool oq_Editable,
                     QGraphicsItem * const opc_Parent = NULL);
   C_GiBiTextElement(const stw_types::uint64 & oru64_ID = 0, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiBiTextElement();

   virtual stw_types::sintn type() const override;
   bool OpenStyleDialog(const bool oq_DarkMode);
   virtual void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;

   virtual void RestoreDefaultCursor(void) override;
   virtual void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;

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
   void m_LoadTextElementData(const stw_opensyde_gui_logic::C_PuiBsTextElement * const opc_Data,
                              const bool oq_DarkMode);
   void m_UpdateTextElementData(stw_opensyde_gui_logic::C_PuiBsTextElement * const opc_Data,
                                const bool oq_DarkMode) const;

   virtual void m_ResizeUpdateItems(const stw_types::float64 of64_DiffWidth,
                                    const stw_types::float64 of64_DiffHeight) override;

   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   virtual bool sceneEventFilter(QGraphicsItem * const opc_Watched, QEvent * const opc_Event) override;

   C_GiText * mpc_TextItem;

private:
   //Avoid call
   C_GiBiTextElement(const C_GiBiTextElement &);
   C_GiBiTextElement & operator =(const C_GiBiTextElement &); //lint !e1511 //we want to hide the base func.

   void m_Init(void);
   void m_HandleTextInteractionMode(const bool & orq_On);

   bool mq_Editable;

   static const stw_types::float64 mhf64_MIN_WIDTH_TEXT_ELEMENT;
   static const stw_types::float64 mhf64_MIN_HEIGHT_TEXT_ELEMENT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
