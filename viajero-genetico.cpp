#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

constexpr int DEFAULT_NODES      = 5;
constexpr int DEFAULT_POP_SIZE   = 10;
constexpr int DEFAULT_INIT_TEMP  = 10000;
constexpr int DEFAULT_MIN_TEMP   = 1000;
constexpr int DEFAULT_MAX_GEN    = 5;
constexpr int DEFAULT_MAX_WEIGHT = 20;
constexpr int DEFAULT_COOL_PCT   = 90;

struct Config {
    int nodes      = DEFAULT_NODES;
    int pop_size   = DEFAULT_POP_SIZE;
    int init_temp  = DEFAULT_INIT_TEMP;
    int min_temp   = DEFAULT_MIN_TEMP;
    int max_gen    = DEFAULT_MAX_GEN;
    int max_weight = DEFAULT_MAX_WEIGHT;
    int cool_pct   = DEFAULT_COOL_PCT;
};

struct Individual {
    std::vector<int> genome;
    int fitness = std::numeric_limits<int>::max();
};

struct SliderConfig {
    std::string label;
    int* ptr;
    int min, max;
    int value;

    SliderConfig() = default;
    SliderConfig(const std::string& lbl, int* p, int mi, int ma)
      : label(lbl), ptr(p), min(mi), max(ma), value(p ? *p : 0) {}
};

struct Slider {
    struct SliderConfig config;
    float x;
    float y;
    float width;
    bool dragging = false;
    sf::RectangleShape track;
    sf::RectangleShape knob;

    Slider() = default;
    Slider(const std::string& lbl, int mn, int mx, int val, float posX, float posY, float width, float height)
      : x(posX), y(posY), width(width)
    {
        config = SliderConfig(lbl, nullptr, mn, mx);
        config.value = val;
    }
};

struct Button {
    std::string label;
    sf::RectangleShape shape;
    sf::Text text;
};

static std::mt19937 rng(std::random_device{}());
static std::vector<std::vector<int>> MAP;

int rand_num(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng);
}

void init_map(int nodes, int max_weight) {
    MAP.assign(nodes, std::vector<int>(nodes, 0));
    for (int i = 0; i < nodes; ++i)
        for (int j = 0; j < nodes; ++j)
            if (i == j)
                MAP[i][j] = 0;
            else if (j < i)
                MAP[i][j] = MAP[j][i];
            else
                MAP[i][j] = rand_num(1, max_weight);
}

int cal_fitness(const std::vector<int>& g) {
    int f = 0;
    for (int i = 0; i + 1 < static_cast<int>(g.size()); ++i) {
        if (MAP[g[i]][g[i + 1]] == std::numeric_limits<int>::max())
            return std::numeric_limits<int>::max();
        f += MAP[g[i]][g[i + 1]];
    }
    return f;
}

std::vector<int> create_genome(int nodes, int start) {
    std::vector<int> g = {start};
    std::vector<int> rest;
    for (int i = 0; i < nodes; ++i)
        if (i != start) rest.push_back(i);
    for (int i = static_cast<int>(rest.size()) - 1; i > 0; --i)
        std::swap(rest[i], rest[rand_num(0, i)]);
    for (int n : rest)
        g.push_back(n);
    g.push_back(start);
    return g;
}

std::vector<int> mutate(std::vector<int> g, int nodes) {
    int r = rand_num(1, nodes - 1);
    int r1 = rand_num(1, nodes - 1);
    while (r1 == r)
        r1 = rand_num(1, nodes - 1);
    std::swap(g[r], g[r1]);
    return g;
}

int cooldown(int temp, int cool_pct) {
    return (cool_pct * temp) / 100;
}

void init_population(std::vector<Individual>& pop, const Config& cfg) {
    pop.clear();
    pop.resize(cfg.pop_size);
    for (auto& ind : pop) {
        ind.genome = create_genome(cfg.nodes, 0);
        ind.fitness = cal_fitness(ind.genome);
    }
}

void compute_best(const std::vector<Individual>& pop, std::vector<int>& best_genome, int& best_fitness) {
    best_fitness = std::numeric_limits<int>::max();
    best_genome.clear();
    for (auto& ind : pop)
        if (ind.fitness < best_fitness) {
            best_fitness = ind.fitness;
            best_genome = ind.genome;
        }
}

std::vector<sf::Vector2f> compute_node_positions(int nodes) {
    constexpr float X_MIN = 60.f,  X_MAX = 1020.f;
    constexpr float Y_MIN = 60.f,  Y_MAX = 760.f;
    constexpr float CX    = (X_MIN + X_MAX) / 2.f;
    constexpr float CY    = (Y_MIN + Y_MAX) / 2.f;

    std::vector<sf::Vector2f> pos(nodes);
    std::uniform_real_distribution<float> rx(X_MIN + 60.f, X_MAX - 60.f);
    std::uniform_real_distribution<float> ry(Y_MIN + 60.f, Y_MAX - 60.f);
    for (int i = 0; i < nodes; ++i)
        pos[i] = {rx(rng), ry(rng)};

    if (nodes < 2) return pos;

    // Rango de pesos para normalizar la longitud ideal de cada arista
    int minW = std::numeric_limits<int>::max(), maxW = 1;
    for (int i = 0; i < nodes; ++i)
        for (int j = i + 1; j < nodes; ++j) {
            minW = std::min(minW, MAP[i][j]);
            maxW = std::max(maxW, MAP[i][j]);
        }
    if (maxW <= minW) maxW = minW + 1;

    auto ideal_len = [&](int i, int j) -> float {
        float t = float(MAP[i][j] - minW) / float(maxW - minW);
        return 55.f + t * 255.f;
    };

    constexpr float K_SPRING  = 0.08f;
    constexpr float K_REPULSE = 6000.f;
    constexpr float DAMPING   = 0.80f;
    constexpr int   ITERS     = 400;

    std::vector<sf::Vector2f> vel(nodes, {0.f, 0.f});

    for (int iter = 0; iter < ITERS; ++iter) {
        std::vector<sf::Vector2f> force(nodes, {0.f, 0.f});

        for (int i = 0; i < nodes; ++i) {
            for (int j = i + 1; j < nodes; ++j) {
                sf::Vector2f d = pos[j] - pos[i];
                float dist = std::sqrt(d.x*d.x + d.y*d.y);
                if (dist < 0.5f) { d = {0.5f, 0.f}; dist = 0.5f; }
                float f = K_SPRING * (dist - ideal_len(i, j));
                sf::Vector2f fv = (d / dist) * f;
                force[i] += fv;
                force[j] -= fv;
            }
        }

        for (int i = 0; i < nodes; ++i) {
            for (int j = i + 1; j < nodes; ++j) {
                sf::Vector2f d = pos[j] - pos[i];
                float dist2 = d.x*d.x + d.y*d.y;
                if (dist2 < 1.f) dist2 = 1.f;
                float f = K_REPULSE / dist2;
                sf::Vector2f fv = (d / std::sqrt(dist2)) * f;
                force[i] -= fv;
                force[j] += fv;
            }
        }

        for (int i = 0; i < nodes; ++i) {
            sf::Vector2f d = {CX - pos[i].x, CY - pos[i].y};
            force[i] += d * 0.004f;
        }

        //Integrar
        for (int i = 0; i < nodes; ++i) {
            vel[i] = (vel[i] + force[i]) * DAMPING;
            pos[i] += vel[i];
            pos[i].x = std::max(X_MIN, std::min(X_MAX, pos[i].x));
            pos[i].y = std::max(Y_MIN, std::min(Y_MAX, pos[i].y));
        }
    }

    if (nodes > 1) {
        float bx0 = pos[0].x, bx1 = pos[0].x;
        float by0 = pos[0].y, by1 = pos[0].y;
        for (auto& p : pos) {
            bx0 = std::min(bx0, p.x); bx1 = std::max(bx1, p.x);
            by0 = std::min(by0, p.y); by1 = std::max(by1, p.y);
        }
        float margin = 80.f;
        float src_w  = bx1 - bx0, src_h = by1 - by0;
        float dst_w  = (X_MAX - X_MIN) - 2.f * margin;
        float dst_h  = (Y_MAX - Y_MIN) - 2.f * margin;
        if (src_w > 1.f && src_h > 1.f) {
            float scale = std::min(dst_w / src_w, dst_h / src_h);
            float ox = X_MIN + margin + (dst_w - src_w * scale) / 2.f;
            float oy = Y_MIN + margin + (dst_h - src_h * scale) / 2.f;
            for (auto& p : pos) {
                p.x = ox + (p.x - bx0) * scale;
                p.y = oy + (p.y - by0) * scale;
            }
        }
    }

    return pos;
}

void draw_slider(sf::RenderWindow& window, Slider& slider, sf::Font& font) {
    slider.track.setFillColor(sf::Color(90, 90, 90));
    slider.track.setPosition(slider.x, slider.y + 12.f);
    slider.track.setSize({slider.width, 8.f});
    window.draw(slider.track);

    float ratio = float(slider.config.value - slider.config.min) / float(slider.config.max - slider.config.min);
    float knobX = slider.x + ratio * (slider.width - 16.f);
    
    slider.knob.setSize({16.f, 24.f});
    slider.knob.setFillColor(slider.dragging ? sf::Color(220, 180, 60) : sf::Color(200, 200, 200));
    slider.knob.setPosition(knobX, slider.y + 6.f);
    window.draw(slider.knob);

    sf::Text label(slider.config.label + ": " + std::to_string(slider.config.value), font, 16);
    label.setFillColor(sf::Color::White);
    label.setPosition(slider.x, slider.y - 4.f);
    window.draw(label);
}

//arista gruesa como un quad entre dos puntos
void draw_thick_edge(sf::RenderWindow& window,
                     sf::Vector2f p1, sf::Vector2f p2,
                     float thickness, sf::Color color)
{
    sf::Vector2f dir = p2 - p1;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.001f) return;
    sf::Vector2f unit  = dir / len;
    sf::Vector2f normal = {-unit.y, unit.x};
    sf::Vector2f off   = normal * (thickness / 2.f);

    sf::ConvexShape quad(4);
    quad.setPoint(0, p1 + off);
    quad.setPoint(1, p2 + off);
    quad.setPoint(2, p2 - off);
    quad.setPoint(3, p1 - off);
    quad.setFillColor(color);
    window.draw(quad);
}

bool mouse_in_rect(const sf::FloatRect& rect, const sf::Vector2f& point) {
    return 
        rect.left <= point.x &&
        point.x <= rect.left + rect.width &&
        rect.top <= point.y &&
        point.y <= rect.top + rect.height
    ;
}

void update_slider_value(Slider& slider, const sf::Vector2f& mousePos) {
    float localX = mousePos.x - slider.x;
    localX = std::max(0.f, std::min(localX, slider.width - 16.f));

    float ratio = localX / (slider.width - 16.f);
    slider.config.value = slider.config.min + int(std::round(ratio * (slider.config.max - slider.config.min)));

    if (slider.config.value < slider.config.min)
        slider.config.value = slider.config.min;
    if (slider.config.value > slider.config.max)
        slider.config.value = slider.config.max;
}

void step_generation(std::vector<Individual>& pop, int nodes, int& temp, int cool_pct, int& generation, std::vector<std::string>& log) {
    std::sort(pop.begin(), pop.end(), [](const Individual& a, const Individual& b){
        return a.fitness < b.fitness;
    });

    std::vector<Individual> next_pop;
    next_pop.reserve(pop.size());

    for (auto& p : pop) {
        while (true) {
            Individual candidate;
            candidate.genome = mutate(p.genome, nodes);
            candidate.fitness = cal_fitness(candidate.genome);
            
            std::ostringstream ss;
            ss << "Gen " << generation << " try: cand=" << candidate.fitness << " parent=" << p.fitness;
            if (candidate.fitness < p.fitness) {
                next_pop.push_back(candidate);
                ss << " -> accepted (better)";
                log.push_back(ss.str());
                break;
            }

            double prob = std::exp(-1.0 * (candidate.fitness - p.fitness) / temp);
            ss << " prob=" << prob;
            if (prob > 0.5) {
                next_pop.push_back(candidate);
                ss << " -> accepted (prob)";
                log.push_back(ss.str());
                break;
            }

            ss << " -> rejected";
            log.push_back(ss.str());
        }
    }

    pop = std::move(next_pop);
    temp = cooldown(temp, cool_pct);
    generation += 1;
}

sf::Font load_font() {
    sf::Font font;

    // rutas a las fuentes
    const std::vector<std::string> paths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
    };

    for (auto& path : paths)
        if (font.loadFromFile(path))
            return font;

    return font;
}

// Helper para dibujar texto alineado a la derecha
void drawRightAlignedText(sf::RenderWindow& window, const sf::Font& font, 
                          const std::string& text, float x, float y, 
                          unsigned int charSize = 16, sf::Color color = sf::Color::White) {
    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(charSize);
    sfText.setFillColor(color);
    sfText.setOrigin(sfText.getLocalBounds().width, 0);
    sfText.setPosition(x, y);
    window.draw(sfText);
}

int main() {
    Config cfg;
    init_map(cfg.nodes, cfg.max_weight);

    std::vector<Individual> population;
    init_population(population, cfg);

    std::vector<sf::Vector2f> nodePositions = compute_node_positions(cfg.nodes);

    sf::RenderWindow window(sf::VideoMode(1400, 840), "TSP Visualizer");
    window.setFramerateLimit(60);

    sf::Font font = load_font();
    if (!font.getInfo().family.size()) {
        std::cerr << "Failed to load a system font. Install DejaVuSans or LiberationSans.\n";
        return 1;
    }

//sliders
    std::vector<Slider> sliders;
    float startY = 80.f;
    float spacing = 60.f;

    std::vector<SliderConfig> graphSliderBindings;
    graphSliderBindings.emplace_back("Nodes", &cfg.nodes, 3, 12);
    graphSliderBindings.emplace_back("Max Weight", &cfg.max_weight, 1, 100);

    std::vector<SliderConfig> gaSliderBindings;
    gaSliderBindings.emplace_back("Population", &cfg.pop_size, 2, 60);
    gaSliderBindings.emplace_back("Init Temp", &cfg.init_temp, 10, 50000);
    gaSliderBindings.emplace_back("Min Temp", &cfg.min_temp, 0, 10000);
    gaSliderBindings.emplace_back("Max Gen", &cfg.max_gen, 1, 50);
    gaSliderBindings.emplace_back("Cool %", &cfg.cool_pct, 50, 99);

    sliders.clear();
    for (size_t i = 0; i < graphSliderBindings.size(); ++i) {
        sliders.push_back({
            graphSliderBindings[i].label,
            graphSliderBindings[i].min,
            graphSliderBindings[i].max,
            graphSliderBindings[i].value,
            1090.f,
            startY + i * spacing,
            260.f,
            40.f
        });
    }
    float gaStartY = startY + graphSliderBindings.size() * spacing + 40.f;
    for (size_t i = 0; i < gaSliderBindings.size(); ++i) {
        sliders.push_back({
            gaSliderBindings[i].label,
            gaSliderBindings[i].min,
            gaSliderBindings[i].max,
            gaSliderBindings[i].value,
            1090.f,
            gaStartY + i * spacing,
            260.f,
            40.f
        });
    }

//botones
    Button randomButton;
    randomButton.shape.setSize({260.f, 40.f});
    randomButton.shape.setPosition(1090.f, 540.f);
    randomButton.shape.setFillColor(sf::Color(100, 180, 220));
    randomButton.text.setFont(font);
    randomButton.text.setString("Random Graph (R)");
    randomButton.text.setCharacterSize(18);
    randomButton.text.setFillColor(sf::Color::Black);
    randomButton.text.setPosition(1100.f, 548.f);

    Button resetButton;
    resetButton.shape.setSize({260.f, 40.f});
    resetButton.shape.setPosition(1090.f, 600.f);
    resetButton.shape.setFillColor(sf::Color(100, 220, 140));
    resetButton.text.setFont(font);
    resetButton.text.setString("Reset Algorithm (Enter)");
    resetButton.text.setCharacterSize(18);
    resetButton.text.setFillColor(sf::Color::Black);
    resetButton.text.setPosition(1100.f, 608.f);

    Button toggleButton;
    toggleButton.shape.setSize({260.f, 40.f});
    toggleButton.shape.setPosition(1090.f, 660.f);
    toggleButton.shape.setFillColor(sf::Color(220, 160, 100));
    toggleButton.text.setFont(font);
    toggleButton.text.setString("Run/Pause (Space)");
    toggleButton.text.setCharacterSize(18);
    toggleButton.text.setFillColor(sf::Color::Black);
    toggleButton.text.setPosition(1100.f, 668.f);

    bool running = false;
    bool pendingStep = false;
    int temperature = cfg.init_temp;
    int generation = 0;
    int best_fitness = std::numeric_limits<int>::max();
    std::vector<int> best_genome;
    compute_best(population, best_genome, best_fitness);
    std::vector<std::string> log;
    log.push_back("Log started");

    sf::Clock stepClock;
    bool graphNeedsReset = false;

    int   animIdx       = 0;          // individuo actualmente resaltado
    sf::Clock animClock;              // controla el ciclo entre individuos
    int   prev_best_fitness = best_fitness;
    sf::Clock bestPulseClock;         // pulso visual cuando mejora el mejor

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    running = !running;
                } else if (event.key.code == sf::Keyboard::R) {
                    init_map(cfg.nodes, cfg.max_weight);
                    nodePositions = compute_node_positions(cfg.nodes);
                    init_population(population, cfg);
                    temperature = cfg.init_temp;
                    generation = 0;
                    running = false;
                    compute_best(population, best_genome, best_fitness);
                    log.clear();
                    log.push_back("Randomized graph and reset population");
                } else if (event.key.code == sf::Keyboard::Enter) {
                    init_population(population, cfg);
                    temperature = cfg.init_temp;
                    generation = 0;
                    running = false;
                    compute_best(population, best_genome, best_fitness);
                    log.clear();
                    log.push_back("Population reset");
                } else if (event.key.code == sf::Keyboard::G) {
                    pendingStep = true;
                } else if (event.key.code == sf::Keyboard::Q) {
                    cfg.nodes = rand_num(3, 12);
                    sliders[0].config.value = cfg.nodes;
                    init_map(cfg.nodes, cfg.max_weight);
                    nodePositions = compute_node_positions(cfg.nodes);
                    init_population(population, cfg);
                    temperature = cfg.init_temp;
                    generation = 0;
                    running = false;
                    compute_best(population, best_genome, best_fitness);
                    log.clear();
                    log.push_back("Randomized node count to " + std::to_string(cfg.nodes));
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}));
                if (mouse_in_rect(randomButton.shape.getGlobalBounds(), mousePos)) {
                    init_map(cfg.nodes, cfg.max_weight);
                    nodePositions = compute_node_positions(cfg.nodes);
                    init_population(population, cfg);
                    temperature = cfg.init_temp;
                    generation = 0;
                    running = false;
                    compute_best(population, best_genome, best_fitness);
                } else if (mouse_in_rect(resetButton.shape.getGlobalBounds(), mousePos)) {
                    init_population(population, cfg);
                    temperature = cfg.init_temp;
                    generation = 0;
                    running = false;
                    compute_best(population, best_genome, best_fitness);
                } else if (mouse_in_rect(toggleButton.shape.getGlobalBounds(), mousePos)) {
                    running = !running;
                }
                for (auto& slider : sliders) {
                    sf::FloatRect knobRect = {slider.knob.getPosition().x, slider.knob.getPosition().y, slider.knob.getSize().x, slider.knob.getSize().y};
                    sf::FloatRect trackRect = {slider.x, slider.y + 12.f, slider.width, 8.f};
                    if (mouse_in_rect(knobRect, mousePos) || mouse_in_rect(trackRect, mousePos)) {
                        slider.dragging = true;
                        update_slider_value(slider, mousePos);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                for (auto& slider : sliders) slider.dragging = false;
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos(window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}));
                for (auto& slider : sliders) {
                    if (slider.dragging) {
                        update_slider_value(slider, mousePos);
                    }
                }
            }
        }

        bool sliderChanged = false;
        int previous_nodes = cfg.nodes;
        int previous_pop = cfg.pop_size;
        int previous_init_temp = cfg.init_temp;
        int previous_min_temp = cfg.min_temp;
        int previous_max_gen = cfg.max_gen;
        int previous_max_weight = cfg.max_weight;
        int previous_cool_pct = cfg.cool_pct;

        cfg.nodes       = sliders[0].config.value;
        cfg.max_weight  = sliders[1].config.value;
        cfg.pop_size    = sliders[2].config.value;
        cfg.init_temp   = sliders[3].config.value;
        cfg.min_temp    = sliders[4].config.value;
        cfg.max_gen     = sliders[5].config.value;
        cfg.cool_pct    = sliders[6].config.value;

        if (previous_nodes != cfg.nodes) {
            init_map(cfg.nodes, cfg.max_weight);
            nodePositions = compute_node_positions(cfg.nodes);
            sliderChanged = true;
        }
        if (previous_max_weight != cfg.max_weight) {
            sliderChanged = true;
        }
        if (previous_pop != cfg.pop_size || previous_init_temp != cfg.init_temp || previous_min_temp != cfg.min_temp || previous_max_gen != cfg.max_gen || previous_cool_pct != cfg.cool_pct) {
            sliderChanged = true;
        }
        if (sliderChanged) {
            // flag para el cambio de población
            init_population(population, cfg);
            log.clear();
            log.push_back("Parameters changed --- population reset");
            temperature = cfg.init_temp;
            generation = 0;
            running = false;
            compute_best(population, best_genome, best_fitness);
        }

        if ((running || pendingStep) && stepClock.getElapsedTime().asMilliseconds() > 400) {
            if (temperature > cfg.min_temp && generation < cfg.max_gen) {
                step_generation(population, cfg.nodes, temperature, cfg.cool_pct, generation, log);
                compute_best(population, best_genome, best_fitness);
                if (best_fitness < prev_best_fitness) {
                    prev_best_fitness = best_fitness;
                    bestPulseClock.restart();
                }
            }
            if (temperature <= cfg.min_temp || generation >= cfg.max_gen) {
                running = false;
            }
            stepClock.restart();
            pendingStep = false;
        }

        window.clear(sf::Color(20, 20, 30));

        sf::RectangleShape panel(sf::Vector2f(350.f, 840.f));
        panel.setPosition(1050.f, 0.f);
        panel.setFillColor(sf::Color(40, 40, 50, 220));
        window.draw(panel);

        sf::Text title("TSP Visualizer", font, 24);
        title.setFillColor(sf::Color::White);
        title.setPosition(1090.f, 10.f);
        window.draw(title);

        sf::Text graphHeading("Graph Parameters", font, 18);
        graphHeading.setFillColor(sf::Color(180, 220, 240));
        graphHeading.setPosition(1090.f, 40.f);
        window.draw(graphHeading);

        sf::Text gaHeading("Genetic Algorithm Parameters", font, 18);
        gaHeading.setFillColor(sf::Color(180, 220, 240));
        gaHeading.setPosition(1090.f, 40.f + 2 * spacing + 20.f);
        window.draw(gaHeading);

        for (auto& slider : sliders) {
            draw_slider(window, slider, font);
        }

        window.draw(randomButton.shape);
        window.draw(randomButton.text);
        window.draw(resetButton.shape);
        window.draw(resetButton.text);
        window.draw(toggleButton.shape);
        window.draw(toggleButton.text);

        sf::Text info("Keyboard:\nR=Random graph\nQ=Randomize nodes\nEnter=Reset\nSpace=Run/Pause\nG=Step", font, 16);
        info.setFillColor(sf::Color(210, 210, 210));
        info.setPosition(1090.f, 720.f);
        window.draw(info);

        float rightX = 1030.f;
        float y = 10.f;
        float lineSpacing = 20.f;

        drawRightAlignedText(window, font, "Generation: " + std::to_string(generation), rightX, y);
        drawRightAlignedText(window, font, "Temperature: " + std::to_string(temperature), rightX, y + lineSpacing);
        drawRightAlignedText(window, font, "Best fitness: " + 
            (best_fitness == std::numeric_limits<int>::max() ? "INF" : std::to_string(best_fitness)), 
            rightX, y + lineSpacing * 2);
        drawRightAlignedText(window, font, "Running: " + std::string(running ? "Yes" : "No"), 
            rightX, y + lineSpacing * 3);

        for (int i = 0; i < cfg.nodes; ++i) {
            for (int j = i + 1; j < cfg.nodes; ++j) {
                float thickness = 1.f;
                sf::Color edgeColor(
                    sf::Uint8(190),
                    sf::Uint8(180),
                    sf::Uint8(130)
                );
                draw_thick_edge(window, nodePositions[i], nodePositions[j], thickness, edgeColor);
            }
        }

        for (int i = 0; i < cfg.nodes; ++i) {
            for (int j = i + 1; j < cfg.nodes; ++j) {
                sf::Vector2f dir = nodePositions[j] - nodePositions[i];
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

                sf::Vector2f normal = {-dir.y / len, dir.x / len}; // perpendicular

                float sign = ((i + j) % 2 == 0) ? 1.f : -1.f;
                float offset = 12.f * sign;

                sf::Vector2f pos = {
                    (nodePositions[i].x + nodePositions[j].x) / 2.f + normal.x * offset,
                    (nodePositions[i].y + nodePositions[j].y) / 2.f + normal.y * offset
                };
                sf::Text edgeWeight(std::to_string(MAP[i][j]), font, 12);
                edgeWeight.setFillColor(sf::Color(200, 200, 100));
                edgeWeight.setPosition(pos.x - 8.f, pos.y - 8.f);
                window.draw(edgeWeight);
            }
        }

        //CAPA 1: fantasmas de toda la poblacion (muy tenues)
        for (auto& ind : population) {
            if (ind.genome.size() < 2) continue;
            for (size_t k = 0; k + 1 < ind.genome.size(); ++k) {
                sf::Vector2f a = nodePositions[ind.genome[k]];
                sf::Vector2f b = nodePositions[ind.genome[k + 1]];
                draw_thick_edge(window, a, b, 1.5f, sf::Color(120, 160, 255, 22));
            }
        }

        //CAPA 2: individuo animado (cicla por la poblacion)
        if (running && !population.empty()) {
            if (animClock.getElapsedTime().asMilliseconds() > 130) {
                animIdx = (animIdx + 1) % static_cast<int>(population.size());
                animClock.restart();
            }
            auto& cur = population[animIdx % population.size()];
            if (cur.genome.size() >= 2) {
                for (size_t k = 0; k + 1 < cur.genome.size(); ++k) {
                    sf::Vector2f a = nodePositions[cur.genome[k]];
                    sf::Vector2f b = nodePositions[cur.genome[k + 1]];
                    draw_thick_edge(window, a, b, 2.5f, sf::Color(60, 220, 200, 160));
                }
                // Etiqueta del individuo en evaluacion
                sf::Text candLabel("Evaluating #" + std::to_string(animIdx)
                                   + "  fitness=" + std::to_string(cur.fitness), font, 13);
                candLabel.setFillColor(sf::Color(60, 220, 200));
                candLabel.setPosition(30.f, 700.f);
                window.draw(candLabel);
            }
        }

        //CAPA 3: mejor ruta (con pulso visual al mejorar)
        if (!best_genome.empty()) {
            float pulse = 0.f;
            if (running) {
                float elapsed = bestPulseClock.getElapsedTime().asSeconds();
                pulse = (elapsed < 1.2f) ? std::abs(std::sin(elapsed * 8.f)) * (1.2f - elapsed) / 1.2f : 0.f;
            }
            float thick   = 3.5f + pulse * 4.f;
            sf::Uint8 alpha = static_cast<sf::Uint8>(200 + pulse * 55.f);
            sf::Color bestColor(240, 110 + static_cast<sf::Uint8>(pulse * 50.f), 50, alpha);

            for (size_t k = 0; k + 1 < best_genome.size(); ++k) {
                sf::Vector2f a = nodePositions[best_genome[k]];
                sf::Vector2f b = nodePositions[best_genome[k + 1]];
                draw_thick_edge(window, a, b, thick, bestColor);
            }
            for (size_t k = 0; k < best_genome.size(); ++k) {
                int n = best_genome[k];
                sf::Text order(std::to_string(k), font, 11);
                order.setFillColor(sf::Color(255, 200, 80));
                order.setPosition(nodePositions[n].x + 20.f, nodePositions[n].y - 8.f);
                window.draw(order);
            }
        }

        for (int i = 0; i < cfg.nodes; ++i) {
            sf::CircleShape nodeShape(18.f);
            nodeShape.setOrigin(18.f, 18.f);
            nodeShape.setPosition(nodePositions[i]);
            nodeShape.setFillColor(sf::Color(70, 130, 220));
            nodeShape.setOutlineThickness(2.f);
            nodeShape.setOutlineColor(sf::Color::White);
            window.draw(nodeShape);

            sf::Text nodeLabel(std::to_string(i), font, 16);
            nodeLabel.setFillColor(sf::Color::White);
            nodeLabel.setPosition(nodePositions[i].x - 6.f, nodePositions[i].y - 10.f);
            window.draw(nodeLabel);
        }

        // pantallita cli
        sf::RectangleShape consoleBg(sf::Vector2f(800.f, 90.f));
        consoleBg.setPosition(30.f, 720.f);
        consoleBg.setFillColor(sf::Color(10, 10, 10, 220));
        consoleBg.setOutlineThickness(1.f);
        consoleBg.setOutlineColor(sf::Color(80, 80, 80));
        window.draw(consoleBg);
        int maxLines = 6;
        int startLine = std::max(0, (int)log.size() - maxLines);
        for (int i = startLine; i < (int)log.size(); ++i) {
            sf::Text lineText(log[i], font, 12);
            lineText.setFillColor(sf::Color(200, 200, 200));
            lineText.setPosition(35.f, 725.f + 14.f * (i - startLine));
            window.draw(lineText);
        }

        window.display();
    }

    return 0;
}