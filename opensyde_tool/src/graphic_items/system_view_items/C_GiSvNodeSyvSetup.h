//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view setup specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GINODESYVSETUP_H
#define C_GINODESYVSETUP_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#include "C_GiSvNodeSyvBase.h"
#include "C_GiCheckBox.h"
#include "C_GiRectPixmap.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvNodeSyvSetup :
   public C_GiSvNodeSyvBase
{
   Q_OBJECT

public:
   C_GiSvNodeSyvSetup(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 & ors32_NodeIndex,
                      const stw_types::uint64 & oru64_ID, const stw_types::float64 & orf64_Width = mhf64_MinWidthNode,
                      const stw_types::float64 & orf64_Height = mhf64_MinHeightNode,
                      QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvNodeSyvSetup();

   virtual void SetViewConnected(const bool oq_Connected) override;
   bool IsViewConnected(void) const;
   void SetEditMode(const bool oq_Active);

   virtual void GenerateHint(void) override;

   virtual void UpdateData(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigViewConnectionChanged(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_GiSvNodeSyvSetup(const C_GiSvNodeSyvSetup &);
   C_GiSvNodeSyvSetup & operator =(const C_GiSvNodeSyvSetup &);

   void m_InitCheckBox(void);
   void m_UpdateCheckBox(void);
   void m_InitInteractiveIcons(const stw_types::uint32 ou32_Count);

   C_GiCheckBox * mpc_CheckBox;
   std::vector<C_GiRectPixmap *> mc_InteractiveIcons;
   C_GiRectPixmap * mpc_HoveredInteractiveIcon;
   bool mq_EditMode;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
