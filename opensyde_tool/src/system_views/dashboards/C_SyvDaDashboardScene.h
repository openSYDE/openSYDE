//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Main graphics scene for a concrete dashboard (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSCENE_H
#define C_SYVDADASHBOARDSCENE_H

/* -- Includes ------------------------------------------------------------- */
#include <C_OgeChaChartBase.h>
#include <QTimer>

#include "C_GiSvDaArrow.h"
#include "C_GiSvDaBoundary.h"
#include "C_GiSvDaRectBaseGroup.h"
#include "C_GiSvDaImageGroup.h"
#include "C_GiSvDaTextElement.h"
#include "C_SebScene.h"
#include "C_PuiSvDashboard.h"
#include "C_SyvDaUnoManager.h"
#include "C_SyvDaCopyPasteManager.h"
#include "C_SyvDaContextMenuManager.h"
#include "C_SyvComDriverDiag.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaDashboardScene :
   public C_SebScene
{
   Q_OBJECT

public:
   C_SyvDaDashboardScene(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_DashboardIndex,
                         const bool oq_LoadDashboard = true, QObject * const opc_Parent = NULL);
   virtual ~C_SyvDaDashboardScene(void);

   virtual bool IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos) override;

   void SetEditMode(const bool oq_Active);
   void SetDashboardIndex(const stw_types::uint32 ou32_DashboardIndex);
   void Load(void);
   void Save(void) const;
   void CopyFromSnapshotToScene(const stw_opensyde_gui_logic::C_PuiSvDashboard & orc_Snapshot,
                                const QPointF * const opc_Pos = NULL,
                                const QMap<stw_opensyde_gui_logic::C_PuiBsTemporaryDataID,
                                           stw_types::uint64> * const opc_IDMap =
                                   NULL);
   void DeleteItem(QGraphicsItem * const opc_Item);

   virtual void SetDarkModeActive(const bool oq_Value) override;
   void SetDarkModeInitialized(void);

   //lint -e{1735} Suppression, because default parameters are identical
   virtual void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) override;
   virtual void UpdateTransform(const QTransform & orc_Transform) override;

   virtual bool IsAnyItemAddable(void) const override;
   virtual bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsItemDeletable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsZOrderChangeable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsSceneRubberBandAvailable(void) const override;

   stw_types::uint32 GetViewIndex(void) const;
   stw_types::uint32 GetDashboardIndex(void) const;

   void RegisterWidgets(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) const;
   void ConnectionActiveChanged(const bool oq_Active) const;
   void UpdateShowValues(void) const;
   void UpdateTransmissionConfiguration(void) const;
   void HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC) const;
   void SetErrorForFailedCyclicElementIdRegistrations(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
      const std::vector<QString> & orc_FailedIdErrorDetails) const;
   void TriggerErrorCheck(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;
   void SigDataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                         const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   void SigDataPoolRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Index);

protected:
   virtual stw_opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;
   virtual C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   virtual stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) override;
   virtual const stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const override;

   // drag and drop function
   virtual void m_AddImage(const QString & orc_Path, const QPointF & orc_Position) override;
   virtual bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) override;

   // copy past functions
   virtual void m_Copy(void);
   virtual void m_Cut(void);
   virtual bool m_IsPastePossible(void) const;
   virtual bool m_IsUndoAvailable(void) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaDashboardScene(const C_SyvDaDashboardScene &);
   C_SyvDaDashboardScene & operator =(const C_SyvDaDashboardScene &);

   //Properties
   void m_OnWidgetEditProperties(QGraphicsItem * const opc_Item) const;

   //Connects
   void m_AddWidgetToScene(C_GiSvDaRectBaseGroup * const opc_Item);

   void m_RemoveWidgetOfScene(C_GiSvDaRectBaseGroup * const opc_Item);
   void m_RemoveBoundaryOfScene(C_GiSvDaBoundary * const opc_Item);
   void m_RemoveTextElementOfScene(C_GiSvDaTextElement * const opc_Item);
   void m_RemoveImageGroupOfScene(C_GiSvDaImageGroup * const opc_Item);
   void m_RemoveLineArrowOfScene(C_GiSvDaArrow * const opc_Item);

   //Sync
   void m_SyncIndex(const stw_opensyde_gui_logic::C_PuiSvDbDataElement::E_Type & ore_Type,
                    const stw_types::sint32 & ors32_Index,
                    const stw_opensyde_gui_logic::C_PuiSvDbDataElement::E_Action & ore_Action) const;

   //Add
   void m_LoadSubset(const QVector<stw_types::uint32> & orc_OtherStartIndices, const QPointF * const opc_Offset,
                     const bool & orq_Selection, const QMap<stw_opensyde_gui_logic::C_PuiBsTemporaryDataID,
                                                            stw_types::uint64> * const opc_IDMap);

   void m_SelectionChanged(void);
   void m_InitBaseSceneContextMenuManager(void);

   C_SyvDaContextMenuManager mc_ContextMenuManager;
   stw_opensyde_gui_logic::C_SyvDaUnoManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SyvDaCopyPasteManager mc_CopyPasteManager;

   bool mq_EditMode;
   const stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_DashboardIndex;
   bool mq_LastKnownDarkMode;
   bool mq_DarkModeInitialized;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
