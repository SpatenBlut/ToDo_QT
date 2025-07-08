#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QStringList>

// Lädt Todos aus Datei
void loadTodos(QListWidget *list) {
    // Datei öffnen
    QFile file("todos.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    // Zeilen lesen und parsen
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;

        // Zeile in Status und Text aufteilen
        QStringList parts = line.split('\t');
        if (parts.size() != 2)
            continue;

        bool checked = (parts[0] == "1");
        QString text = parts[1];

        // Item erstellen und konfigurieren
        QListWidgetItem *item = new QListWidgetItem(text, list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

        // Durchstreichen für erledigte Todos
        QFont font = item->font();
        font.setStrikeOut(checked);
        item->setFont(font);
    }
    file.close();
}

// Speichert alle Todos in Datei
void saveTodos(QListWidget *list) {
    // Datei zum Schreiben öffnen
    QFile file("todos.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    // Alle Items durchgehen und speichern
    QTextStream out(&file);
    for (int i = 0; i < list->count(); ++i) {
        const QListWidgetItem *item = list->item(i);
        QString state = (item->checkState() == Qt::Checked) ? "1" : "0";
        out << state << '\t' << item->text() << '\n';
    }
    file.close();
}

// Hauptfunktion
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // GUI Elemente erstellen
    auto *input  = new QLineEdit;
    input->setPlaceholderText("Neues Todo eingeben...");
    auto *addBtn = new QPushButton("Hinzufügen");
    auto *list   = new QListWidget;

    // Layout aufbauen
    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(input);
    mainLayout->addWidget(addBtn);
    mainLayout->addWidget(list);

    // Fenster konfigurieren
    QWidget window;
    window.setLayout(mainLayout);
    window.resize(400, 300);

    // Todos beim Start laden
    loadTodos(list);

    // Beim Beenden speichern
    QObject::connect(&a, &QApplication::aboutToQuit, [&](){
        saveTodos(list);
    });

    // Neue Todos hinzufügen
    QObject::connect(addBtn, &QPushButton::clicked, [&](){
        const auto text = input->text().trimmed();
        if (text.isEmpty()) return;

        QListWidgetItem *item = new QListWidgetItem(text, list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        input->clear();
    });

    // Durchstreichen bei Status-Änderung
    QObject::connect(list, &QListWidget::itemChanged, [&](QListWidgetItem *item){
        QFont font = item->font();
        font.setStrikeOut(item->checkState() == Qt::Checked);
        item->setFont(font);
    });

    // Anwendung starten
    window.show();
    return a.exec();
}