#include "edit_speed_control_settings_dialog.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QSlider>
#include <QMessageBox>

#include "game_window.hpp"

static constexpr const std::uint32_t TURBO_SLIDER_MAX = 800;
static constexpr const std::uint32_t TURBO_SLIDER_MIN = 100;
static constexpr const std::uint32_t TURBO_SLIDER_GRANULARITY = 25;
static constexpr const std::uint32_t TURBO_SLIDER_TICK_INTERVAL = 100;

static constexpr const std::uint32_t SLOWMO_SLIDER_MAX = 100;
static constexpr const std::uint32_t SLOWMO_SLIDER_MIN = 0;
static constexpr const std::uint32_t SLOWMO_SLIDER_GRANULARITY = 5;
static constexpr const std::uint32_t SLOWMO_SLIDER_TICK_INTERVAL = 25;

static constexpr const std::uint32_t REWIND_SLIDER_MAX = 180;
static constexpr const std::uint32_t REWIND_SLIDER_MIN = 15;
static constexpr const std::uint32_t REWIND_SLIDER_GRANULARITY = 5;
static constexpr const std::uint32_t REWIND_SLIDER_TICK_INTERVAL = 15;

static constexpr const std::uint32_t REWIND_SPEED_SLIDER_MAX = 800;
static constexpr const std::uint32_t REWIND_SPEED_SLIDER_MIN = 0;
static constexpr const std::uint32_t REWIND_SPEED_SLIDER_GRANULARITY = 25;
static constexpr const std::uint32_t REWIND_SPEED_SLIDER_TICK_INTERVAL = 100;

static constexpr const std::uint32_t BASE_SPEED_SLIDER_MAX = 800;
static constexpr const std::uint32_t BASE_SPEED_SLIDER_MIN = 0;
static constexpr const std::uint32_t BASE_SPEED_SLIDER_GRANULARITY = 25;
static constexpr const std::uint32_t BASE_SPEED_SLIDER_TICK_INTERVAL = 100;

EditSpeedControlSettingsDialog::EditSpeedControlSettingsDialog(GameWindow *window) : window(window) {
    this->setWindowTitle("Rewind and Speed Settings");

    auto *layout = new QVBoxLayout(this);
    auto *dialog = this;

    // Align labels
    int label_width = 0;
    std::vector<QWidget *> labels;

    auto add_control = [&layout, &dialog, &label_width, &labels](const char *name, QCheckBox **enable_box, std::vector<std::tuple<const char *, QLineEdit **, QSlider **>> amounts) {
        auto *vwidget = new QGroupBox(dialog);
        vwidget->setTitle(name);
        auto *vlayout = new QVBoxLayout(vwidget);

        // Enabled?
        if(enable_box != nullptr) {
            *enable_box = new QCheckBox("Enabled", vwidget);
            vlayout->addWidget(*enable_box);
        }

        // Amount
        for(auto &i : amounts) {
            auto [label, line_amount, slider_amount] = i;

            auto *amt_widget = new QWidget(vwidget);
            auto *amt_label = new QLabel(label, amt_widget);
            label_width = std::max(label_width, amt_label->sizeHint().width());
            labels.emplace_back(amt_label);

            auto *amt_layout = new QHBoxLayout(amt_widget);
            amt_layout->setContentsMargins(0,0,0,0);
            amt_layout->addWidget(amt_label);

            *slider_amount = new QSlider(Qt::Orientation::Horizontal, amt_widget);
            (*slider_amount)->setTickPosition(QSlider::TickPosition::TicksBelow);
            (*slider_amount)->setMinimumWidth(400);
            amt_layout->addWidget(*slider_amount);

            *line_amount = new QLineEdit(amt_widget);
            amt_layout->addWidget(*line_amount);
            vlayout->addWidget(amt_widget);
        }

        vwidget->setLayout(vlayout);
        layout->addWidget(vwidget);

        return vlayout;
    };

    add_control("Throttle", nullptr, {{ "Base speed (%):", &this->base_speed_amount, &this->base_speed_slider }});
    add_control("Turbo", &this->enable_turbo, {{ "Turbo speed (%):", &this->turbo_amount, &this->turbo_slider }});
    add_control("Slowmo", &this->enable_slowmo, {{ "Slowmo speed (%):", &this->slowmo_amount, &this->slowmo_slider }});
    add_control("Rewind", &this->enable_rewind, {{ "Rewind buffer (sec):", &this->rewind_amount, &this->rewind_slider }, { "Rewind speed (%):", &this->rewind_speed_amount, &this->rewind_speed_slider } });

    this->enable_turbo->setChecked(window->turbo_enabled);
    this->enable_slowmo->setChecked(window->slowmo_enabled);
    this->enable_rewind->setChecked(window->rewind_enabled);

    this->turbo_slider->setMaximum(TURBO_SLIDER_MAX / TURBO_SLIDER_GRANULARITY);
    this->turbo_slider->setTickInterval(TURBO_SLIDER_TICK_INTERVAL / TURBO_SLIDER_GRANULARITY);
    this->turbo_slider->setMinimum(TURBO_SLIDER_MIN / TURBO_SLIDER_GRANULARITY);

    this->rewind_speed_slider->setMaximum(REWIND_SPEED_SLIDER_MAX / REWIND_SPEED_SLIDER_GRANULARITY);
    this->rewind_speed_slider->setTickInterval(REWIND_SPEED_SLIDER_TICK_INTERVAL / REWIND_SPEED_SLIDER_GRANULARITY);
    this->rewind_speed_slider->setMinimum(REWIND_SPEED_SLIDER_MIN / REWIND_SPEED_SLIDER_GRANULARITY);

    this->base_speed_slider->setMaximum(BASE_SPEED_SLIDER_MAX / BASE_SPEED_SLIDER_GRANULARITY);
    this->base_speed_slider->setTickInterval(BASE_SPEED_SLIDER_TICK_INTERVAL / BASE_SPEED_SLIDER_GRANULARITY);
    this->base_speed_slider->setMinimum(BASE_SPEED_SLIDER_MIN / BASE_SPEED_SLIDER_GRANULARITY);

    this->slowmo_slider->setMaximum(SLOWMO_SLIDER_MAX / SLOWMO_SLIDER_GRANULARITY);
    this->slowmo_slider->setTickInterval(SLOWMO_SLIDER_TICK_INTERVAL / SLOWMO_SLIDER_GRANULARITY);
    this->slowmo_slider->setMinimum(SLOWMO_SLIDER_MIN / SLOWMO_SLIDER_GRANULARITY);

    this->rewind_slider->setMaximum(REWIND_SLIDER_MAX / REWIND_SLIDER_GRANULARITY);
    this->rewind_slider->setTickInterval(REWIND_SLIDER_TICK_INTERVAL / REWIND_SLIDER_GRANULARITY);
    this->rewind_slider->setMinimum(REWIND_SLIDER_MIN / REWIND_SLIDER_GRANULARITY);

    connect(this->slowmo_slider, &QSlider::valueChanged, this, &EditSpeedControlSettingsDialog::update_slowmo_textbox);
    connect(this->rewind_slider, &QSlider::valueChanged, this, &EditSpeedControlSettingsDialog::update_rewind_textbox);
    connect(this->turbo_slider, &QSlider::valueChanged, this, &EditSpeedControlSettingsDialog::update_turbo_textbox);
    connect(this->rewind_speed_slider, &QSlider::valueChanged, this, &EditSpeedControlSettingsDialog::update_rewind_speed_textbox);
    connect(this->base_speed_slider, &QSlider::valueChanged, this, &EditSpeedControlSettingsDialog::update_base_speed_textbox);

    this->turbo_amount->setText(QString::number(window->max_turbo * 100));
    this->slowmo_amount->setText(QString::number(window->max_slowmo * 100));
    this->rewind_amount->setText(QString::number(window->rewind_length));
    this->rewind_speed_amount->setText(QString::number(window->rewind_speed * 100));
    this->base_speed_amount->setText(QString::number(window->base_multiplier * 100));

    this->update_sliders();

    (*enable_rewind).setChecked(window->rewind_enabled);
    (*enable_slowmo).setChecked(window->slowmo_enabled);
    (*enable_turbo).setChecked(window->turbo_enabled);

    for(auto &i : labels) {
        i->setMinimumWidth(label_width);
    }


    // OK button
    auto *ok = new QWidget(this);
    auto *ok_layout = new QHBoxLayout(ok);
    ok_layout->setContentsMargins(0,0,0,0);
    ok_layout->addWidget(new QWidget(ok));
    auto *ok_button = new QPushButton("OK", ok);
    ok_button->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    ok_layout->addWidget(ok_button);
    layout->addWidget(ok);
    connect(ok_button, &QPushButton::clicked, this, &EditSpeedControlSettingsDialog::perform_accept);

    this->setLayout(layout);

    this->setFixedSize(this->sizeHint());
}

void EditSpeedControlSettingsDialog::perform_accept() {
    bool rewind_ok = true, rewind_speed_ok = true, base_speed_ok = true, slowmo_ok = true, turbo_ok = true;
    double rewind_amount = this->rewind_amount->text().toDouble(&rewind_ok);
    double rewind_speed_amount = this->rewind_speed_amount->text().toDouble(&rewind_speed_ok) / 100.0;
    double slowmo_amount = this->slowmo_amount->text().toDouble(&slowmo_ok) / 100.0;
    double turbo_amount = this->turbo_amount->text().toDouble(&turbo_ok) / 100.0;
    double base_speed_amount = this->base_speed_amount->text().toDouble(&base_speed_ok) / 100.0;

    // Check if out of range before committing any changes
    #define COMPLAIN_IF_INVALID(b, name) if(b) { \
        QMessageBox(QMessageBox::Icon::Critical, "Invalid " name, "Input was non-numerical or otherwise invalid.\n\nPlease check your input and try again.", QMessageBox::Cancel).exec(); \
        return; \
    }

    COMPLAIN_IF_INVALID(!rewind_ok || rewind_amount < 0, "Rewind Length")
    COMPLAIN_IF_INVALID(!slowmo_ok || turbo_amount < 0, "Slowmo Speed")
    COMPLAIN_IF_INVALID(!turbo_ok || slowmo_amount < 0, "Turbo Speed")
    COMPLAIN_IF_INVALID(!base_speed_ok || base_speed_amount < 0, "Base Speed")
    COMPLAIN_IF_INVALID(!rewind_speed_ok || rewind_speed_amount < 0, "Rewind Speed")

    // Change things
    if(window->rewind_length != rewind_amount) { // perform this check since the rewind gets reset when set_rewind_length is called
        window->rewind_length = rewind_amount;
        window->instance->set_rewind_length(rewind_amount);
    }
    window->max_slowmo = slowmo_amount;
    window->max_turbo = turbo_amount;
    window->rewind_speed = rewind_speed_amount;
    window->base_multiplier = base_speed_amount;

    window->turbo_enabled = this->enable_turbo->isChecked();
    window->slowmo_enabled = this->enable_slowmo->isChecked();
    window->rewind_enabled = this->enable_rewind->isChecked();

    // Temporarily disable speed modifiers if enabled
    window->reset_emulation_speed();

    this->accept();
}

void EditSpeedControlSettingsDialog::update_sliders() {
    this->turbo_slider->blockSignals(true);
    this->slowmo_slider->blockSignals(true);
    this->rewind_slider->blockSignals(true);
    this->rewind_speed_slider->blockSignals(true);
    this->base_speed_slider->blockSignals(true);

    this->turbo_slider->setValue(this->turbo_amount->text().toInt() / TURBO_SLIDER_GRANULARITY);
    this->slowmo_slider->setValue(this->slowmo_amount->text().toInt() / SLOWMO_SLIDER_GRANULARITY);
    this->rewind_slider->setValue(this->rewind_amount->text().toInt() / REWIND_SLIDER_GRANULARITY);
    this->rewind_speed_slider->setValue(this->rewind_speed_amount->text().toInt() / REWIND_SPEED_SLIDER_GRANULARITY);
    this->base_speed_slider->setValue(this->base_speed_amount->text().toInt() / BASE_SPEED_SLIDER_GRANULARITY);

    this->turbo_slider->blockSignals(false);
    this->slowmo_slider->blockSignals(false);
    this->rewind_slider->blockSignals(false);
    this->rewind_speed_slider->blockSignals(false);
    this->base_speed_slider->blockSignals(false);
}

void EditSpeedControlSettingsDialog::update_rewind_textbox(int v) {
    this->rewind_amount->setText(QString::number(v * REWIND_SLIDER_GRANULARITY));
}
void EditSpeedControlSettingsDialog::update_rewind_speed_textbox(int v) {
    this->rewind_speed_amount->setText(QString::number(v * REWIND_SPEED_SLIDER_GRANULARITY));
}
void EditSpeedControlSettingsDialog::update_turbo_textbox(int v) {
    this->turbo_amount->setText(QString::number(v * TURBO_SLIDER_GRANULARITY));
}
void EditSpeedControlSettingsDialog::update_slowmo_textbox(int v) {
    this->slowmo_amount->setText(QString::number(v * SLOWMO_SLIDER_GRANULARITY));
}
void EditSpeedControlSettingsDialog::update_base_speed_textbox(int v) {
    this->base_speed_amount->setText(QString::number(v * BASE_SPEED_SLIDER_GRANULARITY));
}
