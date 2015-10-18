#include "mainwindow.h"

#include "guiserialization.hxx"

struct mainWindowSerializationEnv : public magicSerializationProvider
{
    inline void Initialize( MainWindow *mainWnd )
    {
        RegisterMainWindowSerialization( mainWnd, MAGICSERIALIZE_MAINWINDOW, this );
    }

    inline void Shutdown( MainWindow *mainWnd )
    {
        UnregisterMainWindowSerialization( mainWnd, MAGICSERIALIZE_MAINWINDOW );
    }

    enum eSelectedTheme
    {
        THEME_DARK,
        THEME_LIGHT
    };

    struct mtxd_cfg_struct
    {
        bool addImageGenMipmaps;
        bool lockDownTXDPlatform;
        endian::little_endian <eSelectedTheme> selectedTheme;
        bool showLogOnWarning;
        bool adjustTextureChunksOnImport;
    };

    void Load( MainWindow *mainwnd, rw::BlockProvider& mtxdConfig ) override
    {
        // last directory we were in to save TXD file.
        {
            std::wstring lastTXDSaveDir;

            bool gotDir = RwReadUnicodeString( mtxdConfig, lastTXDSaveDir );

            if ( gotDir )
            {
                mainwnd->lastTXDSaveDir = QString::fromStdWString( lastTXDSaveDir );
            }
        }

        // last directory we were in to add an image file.
        {
            std::wstring lastImageFileOpenDir;

            bool gotDir = RwReadUnicodeString( mtxdConfig, lastImageFileOpenDir );

            if ( gotDir )
            {
                mainwnd->lastImageFileOpenDir = QString::fromStdWString( lastImageFileOpenDir );
            }
        }

        mtxd_cfg_struct cfgStruct;
        mtxdConfig.readStruct( cfgStruct );

        mainwnd->addImageGenMipmaps = cfgStruct.addImageGenMipmaps;
        mainwnd->lockDownTXDPlatform = cfgStruct.lockDownTXDPlatform;

        // Select the appropriate theme.
        eSelectedTheme themeOption = cfgStruct.selectedTheme;

        if ( themeOption == THEME_DARK )
        {
            mainwnd->onToogleDarkTheme( true );
            mainwnd->actionThemeDark->setChecked( true );
        }
        else if ( themeOption == THEME_LIGHT )
        {
            mainwnd->onToogleLightTheme( true );
            mainwnd->actionThemeLight->setChecked( true );
        }

        mainwnd->showLogOnWarning = cfgStruct.showLogOnWarning;
        mainwnd->adjustTextureChunksOnImport = cfgStruct.adjustTextureChunksOnImport;

        // TXD log settings.
        {
            rw::BlockProvider logGeomBlock( &mtxdConfig );

            logGeomBlock.EnterContext();

            try
            {
                if ( logGeomBlock.getBlockID() == rw::CHUNK_STRUCT )
                {
                    int geomSize = (int)logGeomBlock.getBlockLength();

                    QByteArray tmpArr( geomSize, 0 );

                    logGeomBlock.read( tmpArr.data(), geomSize );

                    // Restore geometry.
                    mainwnd->txdLog->restoreGeometry( tmpArr );
                }
            }
            catch( ... )
            {
                logGeomBlock.LeaveContext();

                throw;
            }

            logGeomBlock.LeaveContext();
        }
    }

    void Save( const MainWindow *mainwnd, rw::BlockProvider& mtxdConfig ) const override
    {
        RwWriteUnicodeString( mtxdConfig, mainwnd->lastTXDSaveDir.toStdWString() );
        RwWriteUnicodeString( mtxdConfig, mainwnd->lastImageFileOpenDir.toStdWString() );

        mtxd_cfg_struct cfgStruct;
        cfgStruct.addImageGenMipmaps = mainwnd->addImageGenMipmaps;
        cfgStruct.lockDownTXDPlatform = mainwnd->lockDownTXDPlatform;

        // Write theme.
        eSelectedTheme themeOption = THEME_DARK;

        if ( mainwnd->actionThemeDark->isChecked() )
        {
            themeOption = THEME_DARK;
        }
        else if ( mainwnd->actionThemeLight->isChecked() )
        {
            themeOption = THEME_LIGHT;
        }

        cfgStruct.selectedTheme = themeOption;
        cfgStruct.showLogOnWarning = mainwnd->showLogOnWarning;
        cfgStruct.adjustTextureChunksOnImport = mainwnd->adjustTextureChunksOnImport;

        mtxdConfig.writeStruct( cfgStruct );

        // TXD log properties.
        {
            QByteArray logGeom = mainwnd->txdLog->saveGeometry();

            rw::BlockProvider logGeomBlock( &mtxdConfig );

            logGeomBlock.EnterContext();

            try
            {
                int geomSize = logGeom.size();

                logGeomBlock.write( logGeom.constData(), geomSize );
            }
            catch( ... )
            {
                logGeomBlock.LeaveContext();

                throw;
            }

            logGeomBlock.LeaveContext();
        }
    }
};

static PluginDependantStructRegister <mainWindowSerializationEnv, mainWindowFactory_t> mainWindowSerializationEnvRegister;

void InitializeMainWindowSerializationBlock( void )
{
    mainWindowSerializationEnvRegister.RegisterPlugin( mainWindowFactory );
}