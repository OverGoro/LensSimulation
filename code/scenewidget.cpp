#include "scenewidget.h"
#include "polygonalmodel.h"
#include "ui_scenewidget.h"
#include <QColorDialog>
#include <QDoubleValidator>

SceneWidget::SceneWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SceneWidget)
{
    ui->setupUi(this);
    connect(ui->renderPushButton, &QPushButton::clicked, this, &SceneWidget::setParams);
    connect(ui->renderPushButton, &QPushButton::clicked, this, &SceneWidget::renderSignal);

    connect(ui->updatePhotonMapPushButton, &QPushButton::clicked, this, &SceneWidget::setParams);
    connect(ui->updatePhotonMapPushButton, &QPushButton::clicked, this, &SceneWidget::updateMapSignal);

    connect(ui->objectsListWidget, &QListWidget::itemClicked, this, &SceneWidget::onObjectSelected);
    connect(this, &SceneWidget::applyChanges, this, [this](){this->_objsUpdated = true;});

    connect(ui->addSpherePushButton, &QPushButton::clicked, this, [this](){if (_scene) _scene->addObject(std::make_shared<Sphere>(Vec3(0,0,0), 1, Vec3(1,1,1))); loadScene(_scene);});
    connect(ui->addLenspushButton, &QPushButton::clicked, this, [this](){if (_scene) _scene->addObject(std::make_shared<Lens>(Vec3(1,1,1), Vec3(0,0,1), 2, 1, GraphicParams())); loadScene(_scene);});
    connect(ui->addPolygonPushButton, &QPushButton::clicked, this, [this](){if (_scene) _scene->addObject(std::make_shared<Polygon>(Vec3(0,0,0), Vec3(0,1,0), Vec3(1,0,0), Vec3(1,1,1))); loadScene(_scene);});


    ui->camXLineEdit->setValidator(new QDoubleValidator());
    ui->camYLineEdit->setValidator(new QDoubleValidator());
    ui->camZLineEdit->setValidator(new QDoubleValidator());
    ui->camDXLineEdit->setValidator(new QDoubleValidator());
    ui->camDYLineEdit->setValidator(new QDoubleValidator());
    ui->camDZLineEdit->setValidator(new QDoubleValidator());

    ui->objectParamsWidget->setLayout(new QGridLayout(this));
}

SceneWidget::~SceneWidget()
{
    delete ui;
}
void SceneWidget::loadDrawer(const std::shared_ptr<Drawer> &drawer)
{
    if (drawer)
    {
        _drawer = drawer;
        ui->filterConstantLineEdit->setText(QString::number(_drawer->filterConstant()));
        ui->photonsNumLineEdit->setText(QString::number(_drawer->photonsPerLight()));
        ui->photonsRadiusLineEdit->setText(QString::number(_drawer->indirectLightMaxR()));
    }
}

void SceneWidget::loadScene(const std::shared_ptr<Scene> &scene)
{
    if (scene)
    {
        _objsUpdated = false;
        _scene = scene;
        ui->objectsListWidget->clear();
        _objectMap.clear();

        for (const auto &obj : scene->objects()) {
            auto name = QString::fromStdString(typeid(*obj).name());
            name.remove(0,1);
            QListWidgetItem *item = new QListWidgetItem(name, ui->objectsListWidget);
            _objectMap[item] = obj;
        }

        if (scene->camera())
        {
            ui->camXLineEdit->setText(QString::number(scene->camera()->position().x));
            ui->camYLineEdit->setText(QString::number(scene->camera()->position().y));
            ui->camZLineEdit->setText(QString::number(scene->camera()->position().z));

            ui->camDXLineEdit->setText(QString::number(scene->camera()->direction().x));
            ui->camDYLineEdit->setText(QString::number(scene->camera()->direction().y));
            ui->camDZLineEdit->setText(QString::number(scene->camera()->direction().z));

            ui->camFOVLineEdit->setText(QString::number(scene->camera()->fov()));
        }
    }

}

void SceneWidget::setParams()
{
    if(_scene)
    {
        std::shared_ptr<Camera> cam = std::make_shared<Camera>(Vec3(ui->camXLineEdit->text().toDouble(), ui->camYLineEdit->text().toDouble(), ui->camZLineEdit->text().toDouble()),
                                                               Vec3(ui->camDXLineEdit->text().toDouble(), ui->camDYLineEdit->text().toDouble(), ui->camDZLineEdit->text().toDouble()),
                                                               _scene->camera()->screenDistance(), ui->camFOVLineEdit->text().toDouble());
        _scene->setCamera(cam);
        _drawer->setFilterConstant(ui->filterConstantLineEdit->text().toDouble());
    }
    if (_drawer)
    {
        _drawer->setFilterConstant(ui->filterConstantLineEdit->text().toDouble());
        _drawer->setIndirectLightMaxR(ui->photonsRadiusLineEdit->text().toDouble());
        _drawer->setPhotonsPerLight(ui->photonsNumLineEdit->text().toInt());
    }
}
void SceneWidget::onObjectSelected(QListWidgetItem *item)
{
    clearParamsWidget();
    auto obj = _objectMap[item];
    QWidget *paramsWidget = createParamsWidgetForObject(obj);
    if (paramsWidget) {
        ui->objectParamsWidget->layout()->addWidget(paramsWidget);
    }
}

void SceneWidget::clearParamsWidget()
{
    QLayout *layout = ui->objectParamsWidget->layout();
    while (layout->count() > 0) {
        QWidget *widget = layout->takeAt(0)->widget();
        if (widget) {
            widget->deleteLater();
        }
    }
}

QWidget* SceneWidget::createParamsWidgetForObject(const std::shared_ptr<BaseObject>& obj)
{
    if (auto sphere = std::dynamic_pointer_cast<Sphere>(obj)) {
        return createSphereParamsWidget(sphere);
    } else if (auto polygon = std::dynamic_pointer_cast<Polygon>(obj)) {
        return createPolygonParamsWidget(polygon);
    } else if (auto lens = std::dynamic_pointer_cast<Lens>(obj)) {
        return createThinLensParamsWidget(lens);
    } else if (auto model = std::dynamic_pointer_cast<PolygonalModel>(obj)) {
        return createModelParamsWidget(model);
    }
    return nullptr;
}

QWidget* SceneWidget::createSphereParamsWidget(const std::shared_ptr<Sphere>& sphere)
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    int row = 0;

    // Параметры позиции (в одной строке)
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation); // Устанавливаем стандартную нотацию
    doubleValidator->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max()); // Устанавливаем диапазон значений

    // Создаем QLineEdit для координат
    QLineEdit *posXEdit = new QLineEdit(QString::number(sphere->position().x));
    posXEdit->setValidator(doubleValidator); // Устанавливаем валидатор для posXEdit

    QLineEdit *posYEdit = new QLineEdit(QString::number(sphere->position().y));
    posYEdit->setValidator(doubleValidator); // Устанавливаем валидатор для posYEdit

    QLineEdit *posZEdit = new QLineEdit(QString::number(sphere->position().z));
    posZEdit->setValidator(doubleValidator); // Устанавливаем валидатор для posZEdit    layout->addWidget(new QLabel("Координаты (X, Y, Z):"), row, 0);
    QHBoxLayout *positionLayout = new QHBoxLayout();
    positionLayout->addWidget(posXEdit);
    positionLayout->addWidget(posYEdit);
    positionLayout->addWidget(posZEdit);
    connect(posXEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_center.x = text.replace(",", ".").toDouble();});
    connect(posYEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_center.y = text.replace(",", ".").toDouble();});
    connect(posZEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_center.z = text.replace(",", ".").toDouble();});
    layout->addLayout(positionLayout, 0, 1);
    layout->addWidget(new QLabel("Координаты:"), 0, 0);

    QDoubleValidator *positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setNotation(QDoubleValidator::StandardNotation); // Устанавливаем стандартную нотацию
    positiveDoubleValidator->setRange(0.0, std::numeric_limits<double>::max()); // Устанавливаем диапазон значений (только > 0)

    // Радиус
    QLineEdit *radiusEdit = new QLineEdit(QString::number(sphere->_radius));
    radiusEdit->setValidator(positiveDoubleValidator);
    layout->addWidget(new QLabel("Радиус:"), ++row, 0);
    layout->addWidget(radiusEdit, row, 1);
    connect(radiusEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_radius = text.replace(",", ".").toDouble();});

    // Параметры цвета
    QPushButton *colorButton = new QPushButton("");
    layout->addWidget(new QLabel("Цвет поверхности:"), ++row, 0);
    layout->addWidget(colorButton, row, 1);
    colorButton->setStyleSheet("background-color: " + QColor::fromRgbF(
                                                          sphere->_params._color.x,
                                                          sphere->_params._color.y,
                                                          sphere->_params._color.z
                                                          ).name() + ";");

    connect(colorButton, &QPushButton::clicked, this, [sphere, colorButton]() {
        QColor currentColor = QColor::fromRgbF(
            sphere->_params._color.x,
            sphere->_params._color.y,
            sphere->_params._color.z
            );
        QColor selectedColor = QColorDialog::getColor(currentColor, nullptr, "Выбрать цвет");
        if (selectedColor.isValid()) {
            sphere->_params._color = Vec3(
                selectedColor.redF(),
                selectedColor.greenF(),
                selectedColor.blueF()
                );
            colorButton->setStyleSheet("background-color: " + selectedColor.name() + ";");
        }
    });

    // Прозрачность

    QDoubleValidator *transparencyValidator = new QDoubleValidator(this);
    transparencyValidator->setNotation(QDoubleValidator::StandardNotation); // Устанавливаем стандартную нотацию
    transparencyValidator->setRange(0.0, 1.0); // Устанавливаем диапазон значений (0, 1]

    QLineEdit *transparencyEdit = new QLineEdit(QString::number(sphere->_params._transparency));
    transparencyEdit->setValidator(transparencyValidator);
    layout->addWidget(new QLabel("Прозрачность:"), ++row, 0);
    layout->addWidget(transparencyEdit, row, 1);
    connect(transparencyEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_params._transparency = text.replace(",", ".").toDouble();});

    // Индекс преломления
    QDoubleValidator *refrValidator = new QDoubleValidator(this);
    refrValidator->setNotation(QDoubleValidator::StandardNotation); // Устанавливаем стандартную нотацию
    refrValidator->setRange(0.5, 2.0); // Устанавливаем диапазон значений (0, 1]

    QLineEdit *refractionIndexEdit = new QLineEdit(QString::number(sphere->_params._refractiveIndex));
    refractionIndexEdit->setValidator(refrValidator);

    layout->addWidget(new QLabel("Коэффициент преломления:"), ++row, 0);
    layout->addWidget(refractionIndexEdit, row, 1);
    connect(refractionIndexEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_params._refractiveIndex = text.replace(",", ".").toDouble();});

    // Отражательность
    QLineEdit *reflectivityEdit = new QLineEdit(QString::number(sphere->_params._reflectivity));
    reflectivityEdit->setValidator(transparencyValidator);
    layout->addWidget(new QLabel("Отражающая способность:"), ++row, 0);
    layout->addWidget(reflectivityEdit, row, 1);
    connect(reflectivityEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_params._reflectivity = text.replace(",", ".").toDouble();});


    QPushButton *emissionColorButton = new QPushButton("");
    layout->addWidget(new QLabel("Цвет излучения:"), ++row, 0);
    layout->addWidget(emissionColorButton, row, 1);
    emissionColorButton->setStyleSheet("background-color: " + QColor::fromRgbF(
                                                                  sphere->_params._emission.color.x,
                                                                  sphere->_params._emission.color.y,
                                                                  sphere->_params._emission.color.z
                                                                  ).name() + ";");

    connect(emissionColorButton, &QPushButton::clicked, this, [sphere, emissionColorButton]() {
        QColor currentColor = QColor::fromRgbF(
            sphere->_params._emission.color.x,
            sphere->_params._emission.color.y,
            sphere->_params._emission.color.z
            );
        QColor selectedColor = QColorDialog::getColor(currentColor, nullptr, "Выбрать цвет");
        if (selectedColor.isValid()) {
            sphere->_params._emission.color = Vec3(
                selectedColor.redF(),
                selectedColor.greenF(),
                selectedColor.blueF()
                );
            emissionColorButton->setStyleSheet("background-color: " + selectedColor.name() + ";");
        }
    });

    // Интенсивность излучения (положительное значение)
    QLineEdit *emissionIntensityEdit = new QLineEdit(QString::number(sphere->_params._emission.intensity));
    emissionIntensityEdit->setValidator(positiveDoubleValidator);
    layout->addWidget(new QLabel("Интенсивность излучения:"), ++row, 0);
    layout->addWidget(emissionIntensityEdit, row, 1);
    connect(emissionIntensityEdit, &QLineEdit::textChanged, this, [sphere](QString text){sphere->_params._emission.intensity = text.replace(",", ".").toDouble();});


    widget->setLayout(layout);
    return widget;
}



QWidget* SceneWidget::createPolygonParamsWidget(const std::shared_ptr<Polygon>& polygon)
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);

    // Валидаторы
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    doubleValidator->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

    QDoubleValidator *positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveDoubleValidator->setRange(0.0, std::numeric_limits<double>::max()); // Только > 0

    // Параметры вершин (в одной строке)
    QLineEdit *v0XEdit = new QLineEdit(QString::number(polygon->v0.x));
    v0XEdit->setValidator(doubleValidator);
    QLineEdit *v0YEdit = new QLineEdit(QString::number(polygon->v0.y));
    v0YEdit->setValidator(doubleValidator);
    QLineEdit *v0ZEdit = new QLineEdit(QString::number(polygon->v0.z));
    v0ZEdit->setValidator(doubleValidator);

    layout->addWidget(new QLabel("Вершина 0 (X, Y, Z):"), 0, 0);
    QHBoxLayout *v0Layout = new QHBoxLayout();
    v0Layout->addWidget(v0XEdit);
    v0Layout->addWidget(v0YEdit);
    v0Layout->addWidget(v0ZEdit);
    connect(v0XEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v0.x = text.replace(",", ".").toDouble();});
    connect(v0YEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v0.y = text.replace(",", ".").toDouble();});
    connect(v0ZEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v0.z = text.replace(",", ".").toDouble();});
    layout->addLayout(v0Layout, 0, 1);

    // Вершина 1
    QLineEdit *v1XEdit = new QLineEdit(QString::number(polygon->v1.x));
    v1XEdit->setValidator(doubleValidator);
    QLineEdit *v1YEdit = new QLineEdit(QString::number(polygon->v1.y));
    v1YEdit->setValidator(doubleValidator);
    QLineEdit *v1ZEdit = new QLineEdit(QString::number(polygon->v1.z));
    v1ZEdit->setValidator(doubleValidator);

    layout->addWidget(new QLabel("Вершина 1 (X, Y, Z):"), 1, 0);
    QHBoxLayout *v1Layout = new QHBoxLayout();
    v1Layout->addWidget(v1XEdit);
    v1Layout->addWidget(v1YEdit);
    v1Layout->addWidget(v1ZEdit);
    connect(v1XEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v1.x = text.replace(",", ".").toDouble();});
    connect(v1YEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v1.y = text.replace(",", ".").toDouble();});
    connect(v1ZEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v1.z = text.replace(",", ".").toDouble();});
    layout->addLayout(v1Layout, 1, 1);

    // Вершина 2
    QLineEdit *v2XEdit = new QLineEdit(QString::number(polygon->v2.x));
    v2XEdit->setValidator(doubleValidator);
    QLineEdit *v2YEdit = new QLineEdit(QString::number(polygon->v2.y));
    v2YEdit->setValidator(doubleValidator);
    QLineEdit *v2ZEdit = new QLineEdit(QString::number(polygon->v2.z));
    v2ZEdit->setValidator(doubleValidator);

    layout->addWidget(new QLabel("Вершина 2 (X, Y, Z):"), 2, 0);
    QHBoxLayout *v2Layout = new QHBoxLayout();
    v2Layout->addWidget(v2XEdit);
    v2Layout->addWidget(v2YEdit);
    v2Layout->addWidget(v2ZEdit);
    connect(v2XEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v2.x = text.replace(",", ".").toDouble();});
    connect(v2YEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v2.y = text.replace(",", ".").toDouble();});
    connect(v2ZEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->v2.z = text.replace(",", ".").toDouble();});
    layout->addLayout(v2Layout, 2, 1);

    // Параметры цвета
    QPushButton *colorButton = new QPushButton("");
    layout->addWidget(new QLabel("Цвет поверхности:"), 3, 0);
    layout->addWidget(colorButton, 3, 1);
    colorButton->setStyleSheet("background-color: " + QColor::fromRgbF(
                                                          polygon->_params._color.x,
                                                          polygon->_params._color.y,
                                                          polygon->_params._color.z
                                                          ).name() + ";");

    connect(colorButton, &QPushButton::clicked, this, [polygon, colorButton]() {
        QColor currentColor = QColor::fromRgbF(
            polygon->_params._color.x,
            polygon->_params._color.y,
            polygon->_params._color.z
            );
        QColor selectedColor = QColorDialog::getColor(currentColor, nullptr, "Выбрать цвет");
        if (selectedColor.isValid()) {
            polygon->_params._color = Vec3(
                selectedColor.redF(),
                selectedColor.greenF(),
                selectedColor.blueF()
                );
            colorButton->setStyleSheet("background-color: " + selectedColor.name() + ";");
        }
    });

    // Параметры излучения
    // QPushButton *emissionColorButton = new QPushButton("");
    // layout->addWidget(new QLabel("Цвет излучения:"), 4, 0);
    // layout->addWidget(emissionColorButton, 4, 1);
    // emissionColorButton->setStyleSheet("background-color: " + QColor::fromRgbF(
    //                                                               polygon->_params._emission.color.x,
    //                                                               polygon->_params._emission.color.y,
    //                                                               polygon->_params._emission.color.z
    //                                                               ).name() + ";");

    // connect(emissionColorButton, &QPushButton::clicked, this, [polygon, emissionColorButton]() {
    //     QColor currentColor = QColor::fromRgbF(
    //         polygon->_params._emission.color.x,
    //         polygon->_params._emission.color.y,
    //         polygon->_params._emission.color.z
    //         );
    //     QColor selectedColor = QColorDialog::getColor(currentColor, nullptr, "Выбрать цвет");
    //     if (selectedColor.isValid()) {
    //         polygon->_params._emission.color = Vec3(
    //             selectedColor.redF(),
    //             selectedColor.greenF(),
    //             selectedColor.blueF()
    //             );
    //         emissionColorButton->setStyleSheet("background-color: " + selectedColor.name() + ";");
    //     }
    // });

    // Интенсивность излучения (положительное значение)
    // QLineEdit *emissionIntensityEdit = new QLineEdit(QString::number(polygon->_params._emission.intensity));
    // emissionIntensityEdit->setValidator(positiveDoubleValidator);
    // layout->addWidget(new QLabel("Интенсивность излучения:"), 5, 0);
    // layout->addWidget(emissionIntensityEdit, 5, 1);
    // connect(emissionIntensityEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->_params._emission.intensity = text.replace(",", ".").toDouble();});

    // Прозрачность (в диапазоне (0; 1])
    QDoubleValidator *transparencyValidator = new QDoubleValidator(this);
    transparencyValidator->setNotation(QDoubleValidator::StandardNotation);
    transparencyValidator->setRange(0.0, 1.0); // Устанавливаем диапазон значений (0, 1]

    // QLineEdit *transparencyEdit = new QLineEdit(QString::number(polygon->_params._transparency));
    // transparencyEdit->setValidator(transparencyValidator);
    // layout->addWidget(new QLabel("Прозрачность:"), 6, 0);
    // layout->addWidget(transparencyEdit, 6, 1);
    // connect(transparencyEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->_params._transparency = text.replace(",", ".").toDouble();});

    // // Индекс преломления (в диапазоне [0.5, 2.0])
    // QDoubleValidator *refractiveIndexValidator = new QDoubleValidator(this);
    // refractiveIndexValidator->setNotation(QDoubleValidator::StandardNotation);
    // refractiveIndexValidator->setRange(0.5, 2.0); // Устанавливаем диапазон значений [0.5, 2.0]

    // QLineEdit *refractionIndexEdit = new QLineEdit(QString::number(polygon->_params._refractiveIndex));
    // refractionIndexEdit->setValidator(refractiveIndexValidator);
    // layout->addWidget(new QLabel("Коэффициент преломления:"), 7, 0);
    // layout->addWidget(refractionIndexEdit, 7, 1);
    // connect(refractionIndexEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->_params._refractiveIndex = text.replace(",", ".").toDouble();});

    // Отражательность (в диапазоне (0; 1])
    QLineEdit *reflectivityEdit = new QLineEdit(QString::number(polygon->_params._reflectivity));
    reflectivityEdit->setValidator(transparencyValidator); // Используем тот же валидатор, что и для прозрачности
    layout->addWidget(new QLabel("Отражающая способность:"), 8, 0);
    layout->addWidget(reflectivityEdit, 8, 1);
    connect(reflectivityEdit, &QLineEdit::textChanged, this, [polygon](QString text){polygon->_params._reflectivity = text.replace(",", ".").toDouble();});

    widget->setLayout(layout);
    return widget;
}


QWidget* SceneWidget::createThinLensParamsWidget(const std::shared_ptr<Lens>& lens)
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);

    // Параметры позиции (в одной строке)
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    doubleValidator->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

    QLineEdit *posXEdit = new QLineEdit(QString::number(lens->_position.x));
    posXEdit->setValidator(doubleValidator);
    QLineEdit *posYEdit = new QLineEdit(QString::number(lens->_position.y));
    posYEdit->setValidator(doubleValidator);
    QLineEdit *posZEdit = new QLineEdit(QString::number(lens->_position.z));
    posZEdit->setValidator(doubleValidator);
    layout->addWidget(new QLabel("Координаты (X, Y, Z):"), 0, 0);
    QHBoxLayout *positionLayout = new QHBoxLayout();
    positionLayout->addWidget(posXEdit);
    positionLayout->addWidget(posYEdit);
    positionLayout->addWidget(posZEdit);
    connect(posXEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setPosition(Vec3(text.replace(",", ".").toDouble(), lens->_position.y, lens->_position.z));});
    connect(posYEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setPosition(Vec3(lens->_position.x, text.replace(",", ".").toDouble(), lens->_position.z));});
    connect(posZEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setPosition(Vec3(lens->_position.x, lens->_position.y, text.replace(",", ".").toDouble()));});
    layout->addLayout(positionLayout, 0, 1);

    // Параметры направления (в одной строке)
    QLineEdit *dirXEdit = new QLineEdit(QString::number(lens->_direction.x));
    QLineEdit *dirYEdit = new QLineEdit(QString::number(lens->_direction.y));
    QLineEdit *dirZEdit = new QLineEdit(QString::number(lens->_direction.z));
    dirXEdit->setValidator(doubleValidator);
    dirYEdit->setValidator(doubleValidator);
    dirZEdit->setValidator(doubleValidator);
    layout->addWidget(new QLabel("Направление (X, Y, Z):"), 1, 0);
    QHBoxLayout *directionLayout = new QHBoxLayout();
    directionLayout->addWidget(dirXEdit);
    directionLayout->addWidget(dirYEdit);
    directionLayout->addWidget(dirZEdit);
    connect(dirXEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setDirection(Vec3(text.replace(",", ".").toDouble(), lens->direction().y, lens->direction().z));});
    connect(dirYEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setDirection(Vec3(lens->direction().x, text.replace(",", ".").toDouble(), lens->direction().z));});
    connect(dirZEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setDirection(Vec3(lens->direction().x, lens->direction().y, text.replace(",", ".").toDouble()));});
    layout->addLayout(directionLayout, 1, 1);

    // Радиус кривизны (положительное значение)
    QDoubleValidator *positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveDoubleValidator->setRange(0.0, std::numeric_limits<double>::max()); // Только > 0
    QLineEdit *curveRadiusEdit = new QLineEdit(QString::number(lens->_curveRadius));
    curveRadiusEdit->setValidator(positiveDoubleValidator);
    layout->addWidget(new QLabel("Радиус кривизны:"), 2, 0);
    layout->addWidget(curveRadiusEdit, 2, 1);
    connect(curveRadiusEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setCurveRadius(text.replace(",", ".").toDouble());});

    // Радиус (положительное значение)
    QLineEdit *radiusEdit = new QLineEdit(QString::number(lens->_radius));
    radiusEdit->setValidator(positiveDoubleValidator);
    layout->addWidget(new QLabel("Радиус:"), 3, 0);
    layout->addWidget(radiusEdit, 3, 1);
    connect(radiusEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->setRadius(text.replace(",", ".").toDouble());});

    // Параметры цвета
    QPushButton *colorButton = new QPushButton("");
    layout->addWidget(new QLabel("Цвет поверхности:"), 4, 0);
    layout->addWidget(colorButton, 4, 1);
    colorButton->setStyleSheet("background-color: " + QColor::fromRgbF(
                                                          lens->_params._color.x,
                                                          lens->_params._color.y,
                                                          lens->_params._color.z
                                                          ).name() + ";");

    connect(colorButton, &QPushButton::clicked, this, [lens, colorButton]() {
        QColor currentColor = QColor::fromRgbF(
            lens->_params._color.x,
            lens->_params._color.y,
            lens->_params._color.z
            );
        QColor selectedColor = QColorDialog::getColor(currentColor, nullptr, "Выбрать цвет");
        if (selectedColor.isValid()) {
            lens->_params._color = Vec3(
                selectedColor.redF(),
                selectedColor.greenF(),
                selectedColor.blueF()
                );
            colorButton->setStyleSheet("background-color: " + selectedColor.name() + ";");
        }
    });

    // Параметры излучения
    QPushButton *emissionColorButton = new QPushButton("");
    layout->addWidget(new QLabel("Цвет излучения:"), 5, 0);
    layout->addWidget(emissionColorButton, 5, 1);
    emissionColorButton->setStyleSheet("background-color: " + QColor::fromRgbF(
                                                                  lens->_params._emission.color.x,
                                                                  lens->_params._emission.color.y,
                                                                  lens->_params._emission.color.z
                                                                  ).name() + ";");

    connect(emissionColorButton, &QPushButton::clicked, this, [lens, emissionColorButton]() {
        QColor currentColor = QColor::fromRgbF(
            lens->_params._emission.color.x,
            lens->_params._emission.color.y,
            lens->_params._emission.color.z
            );
        QColor selectedColor = QColorDialog::getColor(currentColor, nullptr, "Выбрать цвет");
        if (selectedColor.isValid()) {
            lens->_params._emission.color = Vec3(
                selectedColor.redF(),
                selectedColor.greenF(),
                selectedColor.blueF()
                );
            emissionColorButton->setStyleSheet("background-color: " + selectedColor.name() + ";");
        }
    });

    // Интенсивность излучения (положительное значение)
    QLineEdit *emissionIntensityEdit = new QLineEdit(QString::number(lens->_params._emission.intensity));
    emissionIntensityEdit->setValidator(positiveDoubleValidator);
    layout->addWidget(new QLabel("Интенсивность излучения:"), 6, 0);
    layout->addWidget(emissionIntensityEdit, 6, 1);
    connect(emissionIntensityEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->_params._emission.intensity = text.replace(",", ".").toDouble();});

    // Прозрачность (в диапазоне (0; 1])
    QDoubleValidator *transparencyValidator = new QDoubleValidator(this);
    transparencyValidator->setNotation(QDoubleValidator::StandardNotation);
    transparencyValidator->setRange(0.0, 1.0); // Устанавливаем диапазон значений (0, 1]

    QLineEdit *transparencyEdit = new QLineEdit(QString::number(lens->_params._transparency));
    transparencyEdit->setValidator(transparencyValidator);
    layout->addWidget(new QLabel("Прозрачность:"), 7, 0);
    layout->addWidget(transparencyEdit, 7, 1);
    connect(transparencyEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->_params._transparency = text.replace(",", ".").toDouble();});

    // Индекс преломления (в диапазоне [0.5, 2.0])
    QDoubleValidator *refractiveIndexValidator = new QDoubleValidator(this);
    refractiveIndexValidator->setNotation(QDoubleValidator::StandardNotation);
    refractiveIndexValidator->setRange(0.5, 2.0); // Устанавливаем диапазон значений [0.5, 2.0]

    QLineEdit *refractionIndexEdit = new QLineEdit(QString::number(lens->_params._refractiveIndex));
    refractionIndexEdit->setValidator(refractiveIndexValidator);
    layout->addWidget(new QLabel("Коэффициент преломления:"), 8, 0);
    layout->addWidget(refractionIndexEdit, 8, 1);
    connect(refractionIndexEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->_params._refractiveIndex = text.replace(",", ".").toDouble();});

    // Отражательность (в диапазоне (0; 1])
    QLineEdit *reflectivityEdit = new QLineEdit(QString::number(lens->_params._reflectivity));
    reflectivityEdit->setValidator(transparencyValidator); // Используем тот же валидатор, что и для прозрачности
    layout->addWidget(new QLabel("Отражающая способность:"), 9, 0);
    layout->addWidget(reflectivityEdit, 9, 1);
    connect(reflectivityEdit, &QLineEdit::textChanged, this, [lens](QString text){lens->_params._reflectivity = text.replace(",", ".").toDouble();});

    widget->setLayout(layout);
    return widget;
}


QWidget* SceneWidget::createModelParamsWidget(const std::shared_ptr<PolygonalModel>& lens)
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);

    // Параметры позиции (в одной строке)
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    doubleValidator->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

    widget->setLayout(layout);
    return widget;
}



