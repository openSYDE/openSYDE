//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with basic functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBSCENE_H
#define C_SEBSCENE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsScene>
#include <QRectF>
#include <QImage>
#include <QGraphicsItem>
#include <QTimer>

#include "stwtypes.h"

#include "C_SebUtil.h"
#include "C_NagToolTip.h"
#include "C_GiBiCustomToolTip.h"
#include "C_SebUnoBaseManager.h"
#include "C_SebBaseContextMenuManager.h"
#include "C_SebBaseCopyPasteManager.h"
#include "C_GiLiLineGroup.h"
#include "C_GiBiRectBaseGroup.h"
#include "C_GiBiArrow.h"
#include "C_GiBiBoundary.h"
#include "C_GiBiImageGroup.h"
#include "C_GiBiTextElement.h"
#include "C_PuiBsTemporaryDataID.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebScene :
   public QGraphicsScene
{
   Q_OBJECT

public:
   C_SebScene(QObject * const opc_Parent = NULL);
   virtual ~C_SebScene();

   void FilterChangableZValues(QList<QGraphicsItem *> & orc_ZValues) const;
   stw_types::float64 GetHighestUsedZValueList(const QList<QGraphicsItem *> & orc_Items) const;

   void SetDrawingBackground(const bool oq_Active);
   QRectF GetMinimumSceneRect(void) const;

   void BlockContextMenu(void);
   void DisplayToolTip(const QPointF & orc_ScenePos);
   void DisplaySpecificItemToolTip(const QPointF & orc_ScenePos);

   virtual bool IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos);

   virtual void UpdateTransform(const QTransform & orc_Transform);

   virtual void SetDarkModeActive(const bool oq_Value);

   //Bend line
   stw_types::sint32 BendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos,
                              const stw_types::sint32 * const opc_Index = NULL);
   stw_types::sint32 RemoveBendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos,
                                    const stw_types::sint32 * const opc_Index = NULL) const;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) = 0;

   QGraphicsItem * GetItemByID(const stw_types::uint64 & oru64_ID) const;
   virtual bool IsAnyItemAddable(void) const = 0;
   virtual bool IsItemMovable(const QGraphicsItem * const opc_Item) const = 0;
   virtual bool IsItemSelectable(const QGraphicsItem * const opc_Item) const = 0;
   virtual bool IsItemDeletable(const QGraphicsItem * const opc_Item) const = 0;
   virtual bool IsZOrderChangeable(const QGraphicsItem * const opc_Item) const = 0;
   virtual bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const = 0;
   virtual bool IsSceneRubberBandAvailable(void) const = 0;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading, const QString & orc_Content,
                       const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type);
   void SigHideToolTip(void);
   void SigTriggerUpdateTransform(void);
   void SigChanged(void);
   void SigProxyWidgetInteractionActive(const bool oq_Active);

protected:
   void m_InitSceneContextMenuManager(void);
   virtual stw_opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) = 0;
   virtual C_SebBaseContextMenuManager * m_GetContextMenuManager(void) = 0;
   virtual stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) = 0;
   virtual const stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const = 0;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * const opc_Event) override;
   virtual void dropEvent(QGraphicsSceneDragDropEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event) override;
   //lint -restore

   virtual void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item);
   virtual void m_AddLineArrowToScene(C_GiBiArrow * const opc_Item);
   virtual void m_AddBoundaryToScene(C_GiBiBoundary * const opc_Item);
   virtual void m_AddImageGroupToScene(C_GiBiImageGroup * const opc_Item);
   virtual void m_AddRectBaseGroupToScene(C_GiBiRectBaseGroup * const opc_Item);
   virtual void m_AddLineGroupToScene(C_GiLiLineGroup * const opc_Item);
   void m_AddAnyItemToScene(QGraphicsItem * const opc_Item);
   void m_SetItemSelectionAndMoveAvailability(QGraphicsItem * const opc_Item) const;

   //Move
   void m_HandleRevertableMove(const QPointF & orc_PositionDifference);
   virtual void m_HandleRevertableResizeLine(const stw_types::sint32 & ors32_InteractionPointIndex,
                                             const QPointF & orc_PositionDifference);
   void m_HandleSubLineMove(const stw_types::sint32 & ors32_SubLineIndex, const QPointF & orc_PositionDifference);
   void m_HandleRevertableResizeRectangle(const QPointF & orc_OldPos, const QSizeF & orc_OldSize,
                                          const QPointF & orc_NewPos, const QSizeF & orc_NewSize);

   // selection handling
   void m_UpdateSelection(QGraphicsItem * const opc_Item, const bool oq_ClearSelection = false);
   void m_SelectAll(void) const;
   bool m_IsRubberBandActive(void) const;

   void m_HandleProxyWidgetInteractionChange(const bool & orq_On);
   virtual void m_AddImage(const QString & orc_Path, const QPointF & orc_Position);
   void m_AddImageWithFileDialog(const QPointF & orc_Pos);
   virtual bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) = 0;

   // helper functions
   void m_MapToGlobal(const QPointF & orc_ScenePos, QPoint & orc_GlobalPos) const;
   void m_MapFromGlobal(const QPoint & orc_GlobalPos, QPointF & orc_ScenePos) const;

   // delete functions
   void m_Delete(const bool oq_NoUserConfirm = false);
   void m_CopyItemsToCopyPasteManager(const QList<QGraphicsItem *> & orc_SelectedItems);
   void m_Clear(void);
   void m_RemoveLineGroupOfScene(C_GiLiLineGroup * const opc_Item);
   void m_RemoveRectBaseGroupOfScene(C_GiBiRectBaseGroup * const opc_Item);

   // copy paste functions
   virtual void m_Copy(void) = 0;
   virtual void m_Cut(void) = 0;
   virtual void m_Paste(const QPointF * const opc_Pos);
   virtual bool m_IsPastePossible(void);
   virtual bool m_IsUndoAvailable(void) const;
   virtual bool m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void m_PasteOfClipBoard(const QPointF * const opc_Pos = NULL);

   //Hint and tool tip
   void m_UpdateHints(void) const;
   void m_HandleHideToolTip(void);
   void m_StartTimerForToolTip(const QPointF & orc_ScenePos, const C_GiBiCustomToolTip * const opc_Item);
   void m_StartTimerForToolTipDirect(void);
   void m_StopToolTip(void);

   //Bend line
   void m_BendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos);
   void m_RemoveBendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos);

   //Setup style
   virtual bool m_CallSetupStyle(QGraphicsItem * const opc_Item) const;
   virtual void m_ApplySetupStyleMultiple(const QList<QGraphicsItem *> & orc_SelectedItems,
                                          const QGraphicsItem * const opc_GuidelineItem) const;

   //Unique ID
   stw_types::uint64 m_GetNewUniqueID(const QMap<
                                         stw_opensyde_gui_logic::C_PuiBsTemporaryDataID,
                                         stw_types::uint64> * const opc_IDMap = NULL,
                                      const stw_types::sint32 os32_Type = -1,
                                      const stw_types::uint32 & oru32_Index = 0);

   bool mq_BlockContextMenu;
   bool mq_ProxyWidgetInteractionActive;
   const C_GiBiCustomToolTip * mpc_CurrentHoverItem;
   QPointF mc_ToolTipPos;
   stw_types::uint64 mu64_LastUnusedUniqueID;
   static const QString mhc_BOUNDARY;
   static const QString mhc_TEXT_ELEMENT;
   static const QString mhc_LINE;
   static const QString mhc_IMAGE;

private:
   //Avoid call
   C_SebScene(const C_SebScene &);
   C_SebScene & operator =(const C_SebScene &);

   QRectF m_ItemsBoundRect(void) const;
   void m_AdaptSceneRect(void);

   void m_MoveSelectedItems(const QPointF & orc_Delta);

   // copy paste functions
   void m_PasteKey(void);
   void m_PasteClick(void);

   // order functions
   void m_Align(const QGraphicsItem * const opc_GuidelineItem,
                const stw_opensyde_gui_logic::E_Alignment & ore_Alignment);
   // ZOrder functions
   void m_BringToFront(void);
   void m_SendToBack(void);
   //Setup style
   void m_SetupStyle(QGraphicsItem * const opc_Item);

   // Tool tip
   void m_TriggerToolTip(void);

   void m_HandleBackground(void);
   void m_OnSelectionChange(void);
   void m_DeleteSlot(void);

   static QString mh_GetImageFileTypesFilter(void);

   QImage mc_ImgDrawBackground;
   QImage mc_ImgDrawBackgroundDark;
   QRectF mc_MinimumSceneRect;
   bool mq_RubberBandActive;
   bool mq_LeftButtonPressed;
   bool mq_DrawCustomBackground;
   bool mq_DarkModeActive;
   QPointF mc_LastKnownMouseScenePosition;

   QTimer mc_ToolTipTimer;

   static const stw_types::float64 mhf64_MovingRange;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
