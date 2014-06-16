#-------------------------------------------------
#
# Project created by QtCreator 2012-07-04T17:29:53
#
#-------------------------------------------------

QT       += core gui sql

TARGET = KDBudgetTracker
TEMPLATE = app


SOURCES += main.cpp\
    Forms/MainWindow.cpp \
    Forms/CategoriesDialog.cpp \
    Forms/EditCategoryDialog.cpp \
    Forms/EditTransactionDialog.cpp \
    Forms/TransactionsDialog.cpp \
    utils.cpp \
    Forms/StatisticsDialog.cpp \
    mydb.cpp \
    mydb_types.cpp

HEADERS  += \
    Forms/MainWindow.h \
    Forms/CategoriesDialog.h \
    Forms/EditCategoryDialog.h \
    Forms/EditTransactionDialog.h \
    Forms/TransactionsDialog.h \
    utils.h \
    Forms/StatisticsDialog.h \
    mydb.h \
    mydb_types.h

FORMS    += \
    Forms/MainWindow.ui \
    Forms/CategoriesDialog.ui \
    Forms/EditCategoryDialog.ui \
    Forms/EditTransactionDialog.ui \
    Forms/TransactionsDialog.ui \
    Forms/StatisticsDialog.ui

OTHER_FILES += \
    schema.sql

RESOURCES += \
    resources.qrc
