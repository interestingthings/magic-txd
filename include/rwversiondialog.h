#pragma once

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include <regex>
#include <string>
#include <sstream>

#include <vector>

#include "qtutils.h"
#include "languages.h"

#include "versionshared.h"

class RwVersionDialog : public QDialog, public magicTextLocalizationItem
{
	MainWindow *mainWnd;

    QPushButton *applyButton;

private:
    struct RwVersionDialogSetSelection : public VersionSetSelection
    {
        inline RwVersionDialogSetSelection( MainWindow *mainWnd, RwVersionDialog *dialog ) : VersionSetSelection( mainWnd )
        {
            this->verDialog = dialog;
        }

        void NotifyUpdate( void ) override
        {
            // When changes are made to the configuration, we have to update the button in the main dialog.
            this->verDialog->UpdateAccessibility();
        }

    private:
        RwVersionDialog *verDialog;
    };

    RwVersionDialogSetSelection versionGUI;

private:
    void UpdateAccessibility( void );

public slots:
    void OnRequestAccept( bool clicked );
    void OnRequestCancel( bool clicked );

public:
	RwVersionDialog( MainWindow *mainWnd );
    ~RwVersionDialog( void );

    void updateContent( MainWindow *mainWnd ) override;

    void SelectGame(unsigned int gameId)
    {
        this->versionGUI.gameSelectBox->setCurrentIndex(gameId);
    }

    void updateVersionConfig();
};