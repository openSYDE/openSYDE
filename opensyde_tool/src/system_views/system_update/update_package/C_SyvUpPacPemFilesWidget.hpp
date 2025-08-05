//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating widget to add PEM files in to List
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPEMFILESWIDGET_H
#define C_SYVUPPACPEMFILESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacPemFilesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacPemFilesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacPemFilesWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpPacPemFilesWidget(void) override;
   void UpdatePemFileWidget(void);
   int32_t GetFilesCount(void) const;
   void SaveUserSettings(void);
   void HandleAddPemFile(void);
   void HandleClearAllPemFiles(void);
   QStringList GetPemFilePaths(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   void SigUpdateFileCounter(void);

protected:
   void dropEvent(QDropEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;

private:
   Ui::C_SyvUpPacPemFilesWidget * mpc_Ui;

   //Avoid call
   C_SyvUpPacPemFilesWidget(const C_SyvUpPacPemFilesWidget &);
   C_SyvUpPacPemFilesWidget & operator =(const C_SyvUpPacPemFilesWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
