//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main graphics scene for a concrete dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSCENE_H
#define C_SYVDADASHBOARDSCENE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>

#include "C_GiSvDaParam.hpp"
#include "C_GiSvDaArrow.hpp"
#include "C_GiSvDaBoundary.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_GiSvDaImageGroup.hpp"
#include "C_GiSvDaTextElement.hpp"
#include "C_SebScene.hpp"
#include "C_PuiSvDashboard.hpp"
#include "C_SyvDaUnoManager.hpp"
#include "C_SyvDaCopyPasteManager.hpp"
#include "C_SyvDaContextMenuManager.hpp"
#include "C_SyvComDriverDiag.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardScene :
   public C_SebScene
{
   Q_OBJECT

public:
   C_SyvDaDashboardScene(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                         const bool oq_LoadDashboard = true, QObject * const opc_Parent = NULL);
   ~C_SyvDaDashboardScene(void) override;

   bool IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos) override;
   bool IsSelectionRelevantForProxyWidgetInteraction(void) const override;
   bool IsOverrideCursorNecessary(const QPointF & orc_ScenePos, Qt::CursorShape & ore_Cursor) const override;

   void SetEditMode(const bool oq_Active);
   void SetDrawingActive(const bool oq_Active) const;
   void SetDashboardIndex(const uint32_t ou32_DashboardIndex);
   void Load(void);
   void Save(void) const;
   void CopyFromSnapshotToScene(const stw::opensyde_gui_logic::C_PuiSvDashboard & orc_Snapshot,
                                const QMap<stw::opensyde_gui_logic::C_PuiBsTemporaryDataId,
                                           uint64_t> * const opc_IdMap =
                                   NULL);
   void DeleteItem(QGraphicsItem * const opc_Item);

   void SetDarkModeActive(const bool oq_Value) override;
   void SetDarkModeInitialized(void);

   void UpdateBoundaries(void) const;

   //lint -e{1735} Suppression, because default parameters are identical
   void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) override;
   void UpdateTransform(const QTransform & orc_Transform) override;

   bool IsAnyItemAddable(void) const override;
   bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;
   bool IsItemDeletable(const QGraphicsItem * const opc_Item) const override;
   bool IsZetOrderChangeable(const QGraphicsItem * const opc_Item) const override;
   bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const override;
   bool IsSceneRubberBandAvailable(void) const override;

   uint32_t GetViewIndex(void) const;
   uint32_t GetDashboardIndex(void) const;

   void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) const;
   void ConnectionActiveChanged(const bool oq_Active) const;
   void UpdateShowValues(void) const;
   void UpdateTransmissionConfiguration(void) const;
   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) const;
   void TriggerErrorCheck(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;
   void SigTriggerUpdateTransmissionConfiguration(void);
   void SigDataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                         const uint16_t ou16_ElementIndex);
   void SigDataPoolRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Index);
   void SigNvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);

protected:
   stw::opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;
   C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) override;
   const stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const override;

   // drag and drop function
   void m_AddImage(const QString & orc_Path, const QPointF & orc_Position) override;
   bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) override;

   // copy past functions
   void m_Copy(void) override;
   void m_Cut(void) override;
   bool m_IsPastePossible(void) override;
   bool m_IsUndoAvailable(void) const override;
   bool m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const override;

   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event) override;
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaDashboardScene(const C_SyvDaDashboardScene &);
   C_SyvDaDashboardScene & operator =(const C_SyvDaDashboardScene &) &; //lint !e1511 //we want to hide the base func.

   //Properties
   void m_OnWidgetEditProperties(QGraphicsItem * const opc_Item) const;
   void m_OnWidgetEditContent(QGraphicsItem * const opc_Item) const;

   //Connects
   void m_AddParamWidgetToScene(C_GiSvDaParam * const opc_Item);
   void m_AddWidgetToScene(C_GiSvDaRectBaseGroup * const opc_Item);

   void m_RemoveWidgetOfScene(const C_GiSvDaRectBaseGroup * const opc_Item);
   void m_RemoveBoundaryOfScene(const C_GiSvDaBoundary * const opc_Item);
   void m_RemoveTextElementOfScene(const C_GiSvDaTextElement * const opc_Item);
   void m_RemoveImageGroupOfScene(const C_GiSvDaImageGroup * const opc_Item);
   void m_RemoveLineArrowOfScene(const C_GiSvDaArrow * const opc_Item);

   //Sync
   void m_SyncIndex(const stw::opensyde_gui_logic::C_PuiSvDbDataElement::E_Type & ore_Type, const int32_t & ors32_Index,
                    const stw::opensyde_gui_logic::C_PuiSvDbDataElement::E_Action & ore_Action) const;

   //Add
   void m_LoadSubset(const QVector<uint32_t> & orc_OtherStartIndices, const bool & orq_Selection,
                     const QMap<stw::opensyde_gui_logic::C_PuiBsTemporaryDataId,
                                uint64_t> * const opc_IdMap);

   void m_SelectionChanged(void);
   void m_InitBaseSceneContextMenuManager(void) const;

   C_SyvDaContextMenuManager mc_ContextMenuManager;
   stw::opensyde_gui_logic::C_SyvDaUnoManager mc_UndoManager;
   stw::opensyde_gui_logic::C_SyvDaCopyPasteManager mc_CopyPasteManager;

   bool mq_EditMode;
   const uint32_t mu32_ViewIndex;
   uint32_t mu32_DashboardIndex;
   bool mq_LastKnownDarkMode;
   bool mq_DarkModeInitialized;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
