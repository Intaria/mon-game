# Слой 1 — Схема данных: ER-диаграмма и спецификация

> **Статус:** согласовано разработчиком (14.10.2026); правки ревью 15.10.2026 внесены (§5, №14–22).
> **Назначение:** единственный источник истины по структуре данных
> (правило закреплено в `AGENTS.md` §5): любое изменение `Data/`, C++ row-структур
> или save-схемы обязано сопровождаться правкой этого документа **в том же коммите**.
> **Форматы:** CSV в `Data/` — истина (ADR-003); импорт в DataTable — производная операция.
> **Обсуждение:** ветка `docs/layer1-data-schema`; комментарии — §8.

---

## 1. Принципы

1. **Шаблон ≠ инстанс.** `species` — определение вида (статичные данные в `Data/`);
   конкретный покемон игрока/мира — `creature_instance` в SaveGame (§4). Никогда не смешивать.
2. **ID — namespaced slug:** `species.bulbasaur`, `ability.flamethrower`,
   `item.potion_s`, `zone.route01_grass`. Стабильны, не переиспользуются;
   переименование = миграция данных. Синтаксис совпадает с GameplayTags (без расхождений в будущем).
3. **Универсальные Condition / Effect** вместо таблицы на каждый кейс — строки в
   `Rules/condition.csv` и `Rules/effect.csv` с полиморфным владельцем
   (`owner_type` + `owner_id`). Семантика по умолчанию — **AND** (все условия владельца);
   варианты «ИЛИ» — несколько строк-владельцев (например, несколько `evolution` от одного вида).
   Новые WIP-механики покрываются новыми *типами* условий/эффектов, а не миграцией структуры.
4. **`tags[]`** на ключевых сущностях — будущие флаги без миграции.
5. **Текст — истина (ADR-003).** Ссылки на ассеты — строковые пути `Content/...` в CSV.
6. **Формулы в C++, параметры — в данных.** Базовые формулы (урон, поимка, рост статов) —
   тестируемый C++ (AGENTS §6); коэффициенты и веса — `Balance/gameplay_config.csv`;
   кривые от уровня — UE CurveTable (CSV-импорт, правка **без пересборки**).
   Выражения-как-данные — только отдельным ADR, если параметров станет мало.
7. **Версионирование сохранений:** в save — `schema_version` (структура) и
   `balance_version` (баланс); миграции — явные функции на C++ по версиям;
   автопересчёт накопленных значений — только там, где правило миграции это говорит явно.

---

## 2. ER-диаграммы

### 2.1 Бой и типы

```mermaid
erDiagram
    TYPE ||--o{ TYPE_CHART : "attacks as"
    TYPE ||--o{ TYPE_CHART : "defends as"
    ABILITY ||--o{ SPECIES_ABILITY : "learnable as"
    SPECIES ||--o{ SPECIES_ABILITY : "can learn"
    ABILITY ||--o{ CONDITION : "learn requirements"
    ABILITY {
        string ability_id PK
        string type_id FK
        string kind "physical | energy"
        uint cooldown_turns
    }
    TYPE {
        string type_id PK
        string color_key
    }
    TYPE_CHART {
        string attacker_type_id PK
        string defender_type_id PK
        float multiplier
    }
    STATUS {
        string status_id PK
        bool persists_after_battle
    }
    NATURE {
        string nature_id PK
        string anim_profile
    }
```

### 2.2 Виды покемонов

```mermaid
erDiagram
    SPECIES ||--o{ SPECIES_STATS : "stats"
    SPECIES ||--o{ SPECIES_ABILITY : "learnset"
    SPECIES ||--o{ EVOLUTION : "evolves from"
    SPECIES ||--o{ EVOLUTION : "evolves to"
    EVOLUTION ||--o{ CONDITION : "criteria"
    SPECIES ||--o{ SPECIES_MUTATION : "allows"
    MUTATION ||--o{ SPECIES_MUTATION : "allowed in"
    SPECIES ||--o{ SPECIES_COSMETIC : "unlocks slots"
    SPECIES ||--o{ SPECIES_FOOD : "prefers"
    SPECIES ||--o{ SPECIES_BREEDING : "partner rules"
    SPECIES ||--o{ SPECIES_BREEDING_GROUP : "egg groups"
    SPECIES ||--o{ SPECIES_ART : "variants"
    SPECIES {
        string species_id PK
        string main_type_id FK
        bool is_legendary
        uint mutation_slots_max
    }
    EVOLUTION {
        string evolution_id PK
        string from_species_id FK
        string to_species_id FK
        int sort_order
    }
```

### 2.3 Предметы и мир

```mermaid
erDiagram
    ITEM ||--o{ EFFECT : "has effects"
    ZONE ||--o{ ZONE_SPAWN : "spawns"
    SPECIES ||--o{ ZONE_SPAWN : "spawned here"
    ZONE ||--o{ ZONE_ENCOUNTER : "bush encounters"
    SPECIES ||--o{ ZONE_ENCOUNTER : "encountered"
    ZONE ||--o{ ZONE_WEATHER : "weather rules"
    WEATHER ||--o{ ZONE_WEATHER : "allowed"
    ZONE_SPAWN ||--o{ CONDITION : "spawn conditions"
    ZONE {
        string zone_id PK
        string kind
        uint max_simultaneous
    }
    ZONE_SPAWN {
        string spawn_id PK
        string species_id FK
        bool despawnable
    }
```

> Границы зон в CSV не хранятся: geometry — volume-актор в уровне со ссылкой `zone_id` (решение №6).

### 2.4 NPC, квесты, активности, механизмы

```mermaid
erDiagram
    NPC }o--o{ DIALOGUE : "can talk"
    DIALOGUE ||--o{ DIALOGUE_NODE : "says"
    DIALOGUE_NODE ||--o{ DIALOGUE_NODE : "branches to"
    NPC ||--o{ SHOP : "owns"
    SHOP ||--o{ SHOP_STOCK : "sells"
    ITEM ||--o{ SHOP_STOCK : "listed"
    NPC }o--o| BATTLE_GROUP : "battles as"
    BATTLE_GROUP ||--o{ BATTLE_GROUP_MEMBER : "has member"
    SPECIES ||--o{ BATTLE_GROUP_MEMBER : "member species"
    BATTLE_GROUP ||--o| TRAINER_RULES : "follows"
    QUEST ||--o{ QUEST_OBJECTIVE : "objectives"
    QUEST ||--o{ REWARD : "grants"
    ACHIEVEMENT ||--o{ REWARD : "grants"
    TOURNAMENT_CONFIG ||--o{ BATTLE_GROUP : "generates"
    CONDITION {
        string owner_type "polymorphic"
        string owner_id
        string condition_type
    }
    EFFECT {
        string owner_type "polymorphic"
        string owner_id
        string effect_type
    }
    REWARD {
        string owner_type "polymorphic"
        string owner_id
        string reward_type
    }
```

> `CONDITION`, `EFFECT`, `REWARD` — полиморфные владельцы (все домены);
> связи к ним на диаграммах других блоков не дублируются ради читаемости.

---

## 3. Спецификация таблиц

Каждая таблица = заголовок «имя + назначение таблицы» (путь к CSV — §6), ниже — **по одной строке на поле**:
колонка «Поле» (имя, `(PK)`/`(FK)`, тип; `?` — может быть пустым, `a\|b\|c` — enum,
`uint` — только неотрицательное, `default` — значение по умолчанию (пустая ячейка CSV = default),
`(uniq)` — уникальная комбинация вне PK) и колонка «Описание» — для чего поле.
Системная информация по всей таблице — в «Заметках» под ней. `name_key`/`desc_key` —
ключи в `Meta/locale.csv` (решение №5). Именование таблиц и колонок — правила
`AGENTS.md` §5 и решения §5 №14–15.

### A. `Data/Types/` и `Data/Battle/`

**`type`** — типы существ и способностей

| Поле | Описание |
|---|---|
| `type_id` (PK, string) | стабильный ID типа (`type.fire`) |
| `name_key` (FK, string) | ключ локализации названия |
| `color_key` (string) | ключ цвета в палитре UI (эталон один, оттенки строит UI) |
| `tags` (tags) | флаги под будущие правила без миграции |

*Заметки: 12 типов (GDD).*

**`type_chart`** — матрица уязвимостей типов друг к другу

| Поле | Описание |
|---|---|
| `attacker_type_id` (PK, FK, string) | тип-нападающий |
| `defender_type_id` (PK, FK, string) | тип-защищающийся |
| `multiplier` (float) | множитель урона: 0 / 0.25 / 0.5 / 1 / 2 / 4 |

*Заметки: **полная сетка 12×12 = 144 строки** (№3); до 2 типов у существа → итог = произведение двух строк.*

**`status`** — боевые статусы и правила их снятия

| Поле | Описание |
|---|---|
| `status_id` (PK, string) | стабильный ID статуса |
| `name_key` (FK, string) | ключ локализации названия |
| `duration_turns` (uint, default 0) | сколько ходов длится; 0 = не снимается само (бессрочно) |
| `persists_after_battle` (bool, default 0) | не проходит после боя |
| `cure_chance_percent` (uint, default 0) | шанс снятия каждый ход, % |
| `tags` (tags) | флаги |

*Заметки: эффекты статуса — строки `Rules/effect` (owner=status); способы лечения — `Rules/effect` (`cure_status`) на зельях/способностях, отдельный `cure_method` не хранится; «не проходит после боя» ✓ GDD.*

**`nature`** — характеры существ: поведение и анимации

| Поле | Описание |
|---|---|
| `nature_id` (PK, string) | стабильный ID характера |
| `name_key` (FK, string) | ключ локализации названия |
| `anim_profile` (enum default\|angry\|happy\|lazy; пусто = базовый) | профиль анимаций поведения |
| `tags` (tags) | флаги |

*Заметки: характер **не влияет на статы** (GDD); влияние на поведение (`boldness_mod`) — отложено (`99-open-questions` #9); `herd` и сон — свойства вида и зоны, не характера.*

### B. `Data/Abilities/`

**`ability`** — способности существ: боевые атаки и навыки вне боя

| Поле | Описание |
|---|---|
| `ability_id` (PK, string) | стабильный ID способности (`ability.flamethrower`) |
| `name_key` (FK, string) | ключ локализации названия |
| `desc_key` (FK, string) | ключ локализации описания |
| `type_id` (FK, string) | тип способности (всегда один) |
| `kind` (enum physical\|energy) | физический или энергетический урон |
| `base_power` (uint) | базовая сила (урон) |
| `accuracy` (uint, default 100) | шанс попадания, % |
| `cooldown_turns` (uint, default 0) | перезарядка, ходов |
| `cast_turns` (uint, default 0) | зарядка, ходов; 0 = применяется мгновенно |
| `recovery_turns` (uint, default 0) | восстановление после применения, ходов |
| `target_scope` (enum self\|enemy\|ally\|any\|all\|none\|random) | кому применяется; разрешение по составу боя (1v1: ally→self, выбор в 2v2) — код резолвера |
| `usable_in_field` (bool, default 0) | можно применять вне боя (в мире) |
| `formula_ref` (string) | ссылка на формулу C++ (принцип 6) |
| `tags` (tags) | флаги |

*Заметки: GDD: тип у каждой атаки один, всегда есть; ≤5 одновременно у покемона, переучивание — runtime. Побочные эффекты, шанс промаха, отложенные эффекты, применение вне боя = строки `Rules/effect` (owner=ability) + колонки выше. **Требования изучения** = `Rules/condition` (owner=ability) — отдельно от `species_ability` вида.*

### C. `Data/Species/` (+ подпапки)

Хаб + child-таблицы (решение №7).

**`species`** — профиль вида: типы, поведение, размеры, ассеты

| Поле | Описание |
|---|---|
| `species_id` (PK, string) | стабильный ID вида (`species.bulbasaur`) |
| `name_key` (FK, string) | ключ локализации имени |
| `desc_key` (FK, string) | ключ локализации описания |
| `main_type_id` (FK, string) | основной, первый тип (обязателен) |
| `additional_type_id` (FK?, string) | второй тип; пусто = монотип |
| `gender` (enum genderless\|male_only\|female_only\|mixed) | пол вида; пропорции — `99-open-questions` #12 |
| `catch_rate_base` (uint) | базовый шанс поимки (вход в формулу) |
| `size_min` (float) | минимальный размер особи |
| `size_max` (float) | максимальный размер особи |
| `tint_base` (hex) | базовый оттенок (его переопределяет зона) |
| `tint_variance` (float; пусто = default из `Balance`) | разброс оттенка HSL ± от `tint_base` |
| `mutation_slots_max` (uint) | максимум одновременных мутаций |
| `perception_radius` (float) | радиус восприятия: с какого расстояния реагирует |
| `behavior` (enum indifferent\|flees\|aggressive) | предпочитаемое поведение (зоны переопределяют) |
| `behavior_mod` (float, default 1) | интенсивность поведения: <1 мягче, >1 сильнее |
| `herd` (bool, default 0) | живёт стадом (дефолт) |
| `sleep_schedule` (enum day\|night\|always\|never) | когда спит: днём, ночью, всегда, не спит |
| `is_legendary` (bool, default 0) | легендарный вид |
| `is_rideable` (bool, default 0) | можно ездить верхом |
| `ride_speed_mod` (float, default 1) | множитель скорости верховой езды |
| `friendship_base` (uint) | стартовое значение дружбы |
| `model_path` (string) | путь к мешу в `Content/` |
| `skeleton_path` (string) | путь к скелету в `Content/` |
| `animation_path` (string) | путь к анимационному набору в `Content/` |
| `tags` (tags) | флаги |

*Заметки: дефолты поведения; зона переопределяет `behavior` и оттенок (GDD); статы — `species_stats`; списки enum-ов — `99-open-questions` #14.*

**`species_stats`** — базовые статы и веса роста вида

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид |
| `hp_base` (uint) | базовый HP — флор, ниже которого вид не опускается |
| `attack_base` (uint) | флор атаки |
| `defense_base` (uint) | флор защиты |
| `sp_attack_base` (uint) | флор спец. атаки |
| `sp_defense_base` (uint) | флор спец. защиты |
| `speed_base` (uint) | флор скорости |
| `hp_weight` (float, default 1) | вес распределения очков пула в HP |
| `attack_weight` (float, default 1) | вес в атаку |
| `defense_weight` (float, default 1) | вес в защиту |
| `sp_attack_weight` (float, default 1) | вес в спец. атаку |
| `sp_defense_weight` (float, default 1) | вес в спец. защиту |
| `speed_weight` (float, default 1) | вес в скорость |

*Заметки: широкая вместо EAV — одна строка на вид; размер пула очков на уровень — `Balance/gameplay_config`; формула от уровня — C++ (принцип 6); сравнение с Pokémon (база фиксирована, IV/EV) — №18; семантика пула «2..100» — `99-open-questions` #13.*

**`species_ability`** — способности, изучаемые видом

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид |
| `ability_id` (PK, FK, string) | изучаемая способность |
| `min_level?` (uint) | минимальный уровень для любого способа изучения |

*Заметки: «у каждого вида свой диапазон навыков» ✓ GDD; способы получения — `Rules/condition` (owner=строка): `level_reached, params=34` — предлагается на уровне, `has_item`/тег — обучение предметом; **новый WIP = новый тип условия**.*

**`evolution`** — варианты эволюции видов

| Поле | Описание |
|---|---|
| `evolution_id` (PK, string) | стабильный ID эволюции |
| `from_species_id` (FK, string) | исходный вид |
| `to_species_id` (FK, string) | вид-результат |
| `sort_order` (int) | порядок вариантов при нескольких эволюциях (для UI) |

*Заметки: «ИЛИ» = несколько строк от одного from; **критерии** (уровень, время суток, дружба, предметы, счётчики, победы…) = `Rules/condition` (owner=evolution), «любой из нескольких предметов» = строки с `combine=any`; жёсткий список to ✓ GDD.*

**`mutation`** — мутации существ: внешние изменения

| Поле | Описание |
|---|---|
| `mutation_id` (PK, string) | стабильный ID мутации |
| `name_key` (FK, string) | ключ локализации названия |
| `visual_ref?` (string) | общий дефолтный визуал (может отсутствовать) |
| `hidden` (bool) | скрытая (не показывать до WIP-размножения) |
| `tags` (tags) | флаги |

*Заметки: стат-модификаторы и модификации значений — `Rules/effect` (owner=mutation); визуал по виду — переопределение `species_mutation.visual_ref`; `hidden` — скрытые мутации из WIP-размножения.*

**`species_mutation`** — допустимые мутации для каждого вида

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид |
| `mutation_id` (PK, FK, string) | допустимая мутация |
| `weight?` (float) | вес выпадения при генерации |
| `visual_ref?` (string) | визуал мутации этого вида (переопределение общего) |

*Заметки: допустимый набор мутаций вида; лимит — `species.mutation_slots_max`.*

**`species_cosmetic`** — косметика, доступная виду

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид |
| `slot_id` (PK, FK → `Meta/cosmetic_slot`) | разблокированный слот внешности |

*Заметки: разблокированные слоты вида.*

**`species_food`** — предпочитаемая видом еда

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид |
| `food_type` (PK, enum) | тип пищи (значения enum — `99-open-questions` #14) |
| `multiplier` (float, default 1) | множитель предпочтения поедания |

*Заметки: предпочтения по еде; любимый конкретный предмет при необходимости добавим отдельной таблицей.*

**`species_breeding`** — правила размножения вида

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид-владелец правил размножения |

*Заметки: **таблица-владелец**: строки `Rules/condition` (owner=`breeding:<species>`) хранят ограничения на партнёра (GDD).*

**`species_breeding_group`** — группы совместимости вида при размножении

| Поле | Описание |
|---|---|
| `species_id` (PK, FK, string) | вид |
| `breeding_type` (PK, enum) | группа совместимости; вид может входить в несколько (значения — `99-open-questions` #14) |

*Заметки: совместимость партнёров — ≥1 общая группа (проверка в коде).*

**`species_art`** — арты вида: базовый и альтернативные

| Поле | Описание |
|---|---|
| `species_art_id` (PK, string) | стабильный ID арта |
| `species_id` (FK, string) | вид |
| `model_ref?` (string) | ссылка на модель; пусто = базовая |
| `texture_ref?` (string) | ссылка на текстуру; пусто = базовая |
| `is_default` (bool, default 0) | базовый арт вида (валидатор: ровно одна строка на вид) |
| `is_wild_spawnable` (bool, default 0) | может ли сгенерироваться в дикой местности |

*Заметки: получение уникальными способами — через события и триггеры (код); **слот под мега-эволюции/формы (WIP)**.*

### D. `Data/Items/`

**`item`** — каталог предметов: категории, цены, стеки

| Поле | Описание |
|---|---|
| `item_id` (PK, string) | стабильный ID предмета |
| `name_key` (FK, string) | ключ локализации имени |
| `desc_key` (FK, string) | ключ локализации описания |
| `category` (enum consumable\|pokeball\|quest\|material\|cosmetic) | категория предмета |
| `price` (uint) | цена (одинаковая на покупку и продажу) |
| `is_stackable` (bool, default 1) | стекуется |
| `max_stack` (uint) | максимум в стеке |
| `is_sellable` (bool, default 1) | можно продать |
| `icon_ref` (string) | путь к иконке в `Content/` |
| `tags` (tags) | флаги |

*Заметки: **одна цена на покупку и продажу** (GDD), одинаковая для всех NPC; квестовые (`quest`) нельзя продать — валидатор проверяет `is_sellable=0`; эффекты (зелья/ягоды/покеболы→capture_bonus/эволюционные камни→trigger_evolution/косметика→equip_slot) = `Rules/effect` (owner=item).*

**`craft_recipe`** — рецепты крафта (указатели на правила)

| Поле | Описание |
|---|---|
| `recipe_id` (PK, string) | стабильный ID рецепта |
| `name_key` (FK, string) | ключ локализации названия |
| `rules` (string, path) | путь к файлу правил: комбинации ингредиентов и результаты |
| `tags` (tags) | флаги |

*Заметки: файл `rules` — строки `inputs | outputs | minigame_id? | tags`; выражения вида `1 item.potion_s + 2 any:fish` — много результатов и альтернативных комбинаций без отдельной таблицы; один файл можно переиспользовать между рецептами; разблокировка — `Rules/condition` (owner=craft_recipe); эпик крафта — отдельный слой (GDD).*

### E. `Data/Meta/` и `Data/Balance/`

**`gameplay_config`** — константы баланса, правятся без пересборки

| Поле | Описание |
|---|---|
| `key` (PK, string) | имя константы |
| `value` (string) | значение (типизируется при чтении) |
| `comment` (string) | пояснение к константе |

*Заметки: константы: `party_size=6`, `ability_slots_max=5`, `active_creatures_max=2`, дистанции и время деспавна, пул очков на уровень, default для `tint_variance`, параметры формул, коэффициенты поимки (правка без пересборки, принцип 6).*

**`currency`** — валюты игры

| Поле | Описание |
|---|---|
| `currency_id` (PK, string) | стабильный ID валюты |
| `name_key` (FK, string) | ключ локализации названия |
| `icon_ref` (string) | путь к иконке |
| `tags` (tags) | флаги |

*Заметки: «деньги и прочие валюты» (GDD).*

**`appearance_option`** — варианты внешности игрока и NPC

| Поле | Описание |
|---|---|
| `option_id` (PK, string) | стабильный ID варианта |
| `category` (enum hair\|eyes\|face\|makeup\|body) | часть внешности |
| `visual_ref` (string) | ссылка на ассет |
| `gender?` (enum male\|female) | ограничение по полу; пусто = оба |
| `tags` (tags) | флаги |

*Заметки: каталог вариантов внешности игрока и NPC.*

**`cosmetic_slot`** — слоты косметики: персонаж и покемон

| Поле | Описание |
|---|---|
| `slot_id` (PK, string) | стабильный ID слота |
| `domain` (enum character\|creature) | слот персонажа или покемона |
| `sort` (int) | порядок отображения |

*Заметки: фиксированные списки слотов из GDD (персонаж 12, покемон 6).*

**`locale`** — строки локализации (ru/en)

| Поле | Описание |
|---|---|
| `key` (PK, string) | ключ строки локализации |
| `ru` (string) | русский текст (язык дизайна) |
| `en?` (string) | английский; пусто = не переведено |

*Заметки: отдельный слой локализации (№5).*

### F. `Data/Zones/` и `Data/World/`

**`zone`** — зоны мира: спавн, кусты, погода

| Поле | Описание |
|---|---|
| `zone_id` (PK, string) | стабильный ID зоны (`zone.route01_grass`) |
| `name_key` (FK, string) | ключ локализации названия |
| `kind` (enum spawn\|encounter\|weather\|mixed) | роль зоны |
| `max_simultaneous?` (uint) | максимум особей одновременно во всей зоне |
| `weather_interval_min_seconds?` (uint) | минимальный интервал смены погоды, с |
| `weather_interval_max_seconds?` (uint) | максимальный интервал смены погоды, с |
| `overrides` (string) | какие дефолты вида переопределяет (напр. `override_tint`) |
| `tags` (tags) | флаги |

*Заметки: **конфиг в CSV, геометрия — volume-актор в уровне со ссылкой `zone_id`** (№6); общие правила зоны и их конкатенация с правилами типа зоны — отложены (`99-open-questions` #10).*

**`zone_spawn`** — спавн видов в зонах

| Поле | Описание |
|---|---|
| `spawn_id` (PK, string) | стабильный ID строки спавна |
| `zone_id` (FK, string) | зона |
| `species_id` (FK, string) | вид |
| `weight` (uint) | вес выпадения при генерации спавна |
| `level_min?` (uint) | минимальный уровень особи |
| `level_max?` (uint) | максимальный уровень особи |
| `active_time` (enum day\|night\|any, default any) | когда строка активна |
| `can_sleep` (bool, default 1) | могут ли особи этой строки спать в зоне |
| `despawnable` (bool, default 1) | исчезает ли особь |
| `override_tint?` (hex) | переопределение оттенка вида в зоне |
| `override_behavior?` (enum indifferent\|flees\|aggressive) | переопределение поведения вида в зоне |

*Заметки: условия — `Rules/condition` (owner=строка); лимит особей — `zone.max_simultaneous`; глобальные параметры деспавна — `Balance`; **легендарные ставятся на карту вручную**, спавн-строк для них нет (GDD).*

**`zone_encounter`** — случайные встречи в кустах

| Поле | Описание |
|---|---|
| `encounter_id` (PK, string) | стабильный ID строки |
| `zone_id` (FK, string) | зона |
| `species_id` (FK, string) | вид |
| `level_min` (uint) | минимальный уровень |
| `level_max` (uint) | максимальный уровень |
| `weight` (uint) | вес выпадения |

*Заметки: «кусты» — случайные нападения; условия — `Rules/condition` (owner=строка).*

**`zone_weather`** — погодный режим зон

| Поле | Описание |
|---|---|
| `zone_id` (PK, FK, string) | зона |
| `weather_id` (PK, FK, string) | допустимая погода |
| `weight` (uint) | вес выпадения |
| `is_default` (bool, default 0) | погода по умолчанию в зоне |

*Заметки: интервалы ротатора — колонки `zone.weather_interval_*`; веса и дефолт — построчно (GDD).*

**`weather`** — типы погоды в мире

| Поле | Описание |
|---|---|
| `weather_id` (PK, string) | стабильный ID погоды |
| `name_key` (FK, string) | ключ локализации названия |
| `vfx_ref` (string) | ссылка на VFX-эффект |
| `tags` (tags) | флаги |

*Заметки: боевые/мир-эффекты — `Rules/effect` (owner=weather).*

**`time_of_day`** — смена времён суток в мире

| Поле | Описание |
|---|---|
| `phase_id` (PK, string) | ID фазы; значения = enum фаз из кода |
| `duration_seconds` (uint) | длительность фазы, с |
| `light_preset` (string) | пресет освещения |

*Заметки: таблица-конфигуратор фаз — длительность и свет правятся без пересборки; условие `time_of_day` читает отсюда; список фаз даёт код.*

### G. `Data/NPCs/` и `Data/Quests/`

**`npc`** — персонажи мира и их взаимодействия

| Поле | Описание |
|---|---|
| `npc_id` (PK, string) | стабильный ID NPC |
| `name_key` (FK, string) | ключ локализации имени |
| `appearance[]` (option_id list) | варианты внешности из каталога |
| `battle_trigger` (enum none\|dialogue\|aggressive\|both) | как начинается бой: сам нападает / по диалогу / оба / нет |
| `battle_cooldown_seconds` (uint) | кулдаун между боями, с |
| `battle_group_id` (FK?, string) | команда для боя; пусто = без боя |
| `tags` (tags) | флаги |

*Заметки: **набор** взаимодействий одновременно (talk/battle/trade/quest — не один enum); магазины — `shop` (1:N), диалоги — связующая `npc_dialogue`; иконки взаимодействий считает код; пресеты одежды — `99-open-questions` #11; placement — anchor в уровне.*

**`shop`** — магазины NPC

| Поле | Описание |
|---|---|
| `shop_id` (PK, string) | стабильный ID магазина |
| `npc_id` (FK, string) | владелец-продавец |
| `restock_cooldown_seconds?` (uint) | как часто восстанавливаются позиции; пусто = не восстанавливаются |
| `tags` (tags) | флаги |

*Заметки: товары — `shop_stock`; порядок и лимиты — там же.*

**`dialogue`** — диалоги (владельцы реплик)

| Поле | Описание |
|---|---|
| `dialogue_id` (PK, string) | стабильный ID диалога |
| `name_key` (FK, string) | ключ локализации названия |
| `tags` (tags) | флаги |

*Заметки: выбор активного диалога — `Rules/condition` (owner=dialogue); привязка к NPC — `npc_dialogue`; запуск не от NPC и повторы — `99-open-questions` #8.*

**`npc_dialogue`** — диалоги, доступные NPC

| Поле | Описание |
|---|---|
| `npc_id` (PK, FK, string) | персонаж |
| `dialogue_id` (PK, FK, string) | диалог |

**`dialogue_node`** — реплики и ветвление диалогов

| Поле | Описание |
|---|---|
| `node_id` (PK, string) | стабильный ID реплики |
| `dialogue_id` (FK, string) | диалог-владелец |
| `prev_node_id?` (FK, string) | родительская реплика; её дочерние = варианты ответов; пусто = старт диалога |
| `sort_order` (int) | порядок вариантов среди дочерних |
| `text_key` (FK, string) | текст реплики → `Meta/locale` |
| `action?` (enum none\|open_shop\|give_quest\|start_battle\|end_dialogue) | действие при выборе реплики (значения — `99-open-questions` #14) |
| `params?` (string) | параметры действия |

*Заметки: ветвление — дети по `prev_node_id` (несколько дочерних = варианты ответов, порядок — `sort_order`); условия показа и квест-триггеры — `Rules/condition` (owner=node).*

**`shop_stock`** — товары на полках магазинов

| Поле | Описание |
|---|---|
| `stock_id` (PK, string) | стабильный ID позиции |
| `shop_id` (FK, string) | магазин-владелец |
| `item_id` (FK, string) | товар |
| `count?` (uint) | лимит покупок; пусто = без лимита |
| `is_restockable` (bool, default 0) | восстанавливается ли позиция (период — `shop.restock_cooldown_seconds`) |
| `sort_order` (int) | порядок отображения |

*Заметки: условия появления позиции — `Rules/condition` (owner=строка).*

**`battle_group`** — шаблоны боевых команд

| Поле | Описание |
|---|---|
| `battle_group_id` (PK, string) | стабильный ID группы |
| `rules_id` (FK → `trainer_rules`) | правила боя |
| `tags` (tags) | метки для пулов генерации |

*Заметки: команда-шаблон: NPC (ссылкой `npc.battle_group_id`) и генеративные турниры используют один механизм; группам без тренера NPC не требуется.*

**`battle_group_member`** — состав боевой команды: слоты и бойцы

| Поле | Описание |
|---|---|
| `member_id` (PK, string) | стабильный ID строки |
| `battle_group_id` (FK, string) | группа |
| `slot` (uint) | номер слота в команде |
| `species_id` (FK, string) | вид бойца |
| `level` (uint) | уровень бойца |
| `ability_ids[]` (string list) | изученные способности (≤5) |
| `nature_id?` (FK, string) | характер |
| `tags` (tags) | флаги |

*Заметки: списки — редний контент, валидатор проверяет каждый id; держимые предметы не используются (GDD); живой пул существ NPC — `99-open-questions` #7.*

**`trainer_rules`** — ограничения боёв тренеров

| Поле | Описание |
|---|---|
| `rules_id` (PK, string) | стабильный ID правил |
| `format` (enum solo\|double\|tournament) | вид боя (значения — `99-open-questions` #14) |
| `max_legendaries` (uint) | максимум легендарных в команде |
| `level_cap` (uint) | потолок уровня |

*Заметки: ограничения боя выставляются вручную (GDD); награды = `reward` (owner=trainer_rules).*

**`quest`** — квесты: типы, выдача, повторы

| Поле | Описание |
|---|---|
| `quest_id` (PK, string) | стабильный ID квеста |
| `kind` (enum story\|daily\|weekly) | тип квеста |
| `is_repeatable` (bool, default 0) | можно ли повторять |
| `cooldown_seconds` (uint) | кулдаун повтора, с |
| `npc_id` (FK?, string) | NPC-задатчик; пусто = не от NPC |
| `params?` (string) | пул random-целей для daily/weekly (конфиг генератора) |
| `tags` (tags) | флаги |

*Заметки: **1 NPC → много квестов**; prerequisites и прочие требования — `Rules/condition` (owner=quest, напр. `quest_done`); награды — `reward` (owner=quest).*

**`quest_objective`** — цели квестов: что и сколько выполнить

| Поле | Описание |
|---|---|
| `objective_id` (PK, string) | стабильный ID цели |
| `quest_id` (FK, string) | квест |
| `seq` (uint) | порядок целей (этапы = блоки по `seq`, если понадобятся) |
| `type` (enum defeat\|collect\|visit\|talk\|…) | тип цели |
| `target_id` (string) | цель (вид, предмет, NPC, зона…) |
| `count` (uint) | сколько выполнить |

*Заметки: случайные переменные daily/weekly — через `quest.params` + пул вида/типа (GDD).*

**`reward`** — универсальные награды: квесты, NPC, события

| Поле | Описание |
|---|---|
| `reward_id` (PK, string) | стабильный ID строки награды |
| `owner_type` (enum, см. §3 R) | домен-владелец |
| `owner_id` (string) | ID владельца в домене |
| `reward_type` (enum item\|currency\|achievement\|unlock) | тип награды |
| `target_id` (string) | что выдаётся |
| `count` (uint) | сколько выдаётся |
| `chance` (float) | шанс выпадения, 0..1 |

*Заметки: **общий механизм наград**: квесты, NPC, турниры (WIP-место), залы (WIP), достижения, активности.*

### H. `Data/Activities/`

**`tournament_config`** — конфигурация генеративных турниров

| Поле | Описание |
|---|---|
| `config_id` (PK, string) | стабильный ID конфига |
| `kind` (enum) | вид турнира — выбирает логику в коде (значения — `99-open-questions` #14) |
| `config` (string, path) | путь к файлу конфигурации (число тренеров, level range, правила) |
| `tags` (tags) | метки для пулов |

*Заметки: цепочка тренеров — пулы `battle_group` по tags; награды = `reward` (owner=tournament_config) — **место под WIP «Награды»**.*

**`roguelike_cave_config`** — правила процедурных пещер

| Поле | Описание |
|---|---|
| `config_id` (PK, string) | стабильный ID конфига |
| `kind` (enum) | вид пещеры — выбирает логику в коде (значения — `99-open-questions` #14) |
| `config` (string, path) | путь к файлу конфигурации (длина пути, что сохраняется между заходами и т.п.) |
| `tags` (tags) | флаги |

*Заметки: награды = `reward` (owner=roguelike_cave_config); стартовый выбор существа — механизм дорабатывается (`99-open-questions` #4).*

**`contest_config`** — конфигурация конкурсов

| Поле | Описание |
|---|---|
| `config_id` (PK, string) | стабильный ID конфига |
| `kind` (enum) | вид конкурса — выбирает логику в коде (значения — `99-open-questions` #14) |
| `config` (string, path) | путь к файлу конфигурации (мини-игра, параметры подсчёта) |
| `tags` (tags) | флаги |

*Заметки: **WIP** (реакционная механика).*

**`minigame`** — общие мини-игры: реакция и крафт

| Поле | Описание |
|---|---|
| `minigame_id` (PK, string) | стабильный ID мини-игры |
| `kind` (enum reaction\|craft) | тип мини-игры |
| `config` (string, path) | путь к файлу с параметрами/кривыми сложности |
| `tags` (tags) | флаги |

*Заметки: **WIP**; общий для крафта и конкурса.*

**`achievement`** — достижения: категории и иконки

| Поле | Описание |
|---|---|
| `achievement_id` (PK, string) | стабильный ID достижения |
| `name_key` (FK, string) | ключ локализации названия |
| `desc_key` (FK, string) | ключ локализации описания |
| `category` (enum) | категория (значения — `99-open-questions` #14) |
| `icon_ref` (string) | путь к иконке в `Content/` |
| `hidden` (bool, default 0) | скрыто до выполнения |
| `tags` (tags) | флаги |

*Заметки: условия — conditions (owner=achievement); награды — reward (item/постоянный эффект ✓ GDD).*

**`event`** — временные события игры

| Поле | Описание |
|---|---|
| `event_id` (PK, string) | стабильный ID события |
| `name_key` (FK, string) | ключ локализации названия |
| `config` (string, path) | путь к файлу конфигурации (тайминги, контент) |
| `tags` (tags) | флаги |

*Заметки: награды — `reward` (owner=event); основной источник альт-артов (GDD); детали механики — `99-open-questions` #3.*

### R. `Data/Rules/` — универсальные механизмы (решение №2)

**`condition`** — универсальные условия для любых владельцев

| Поле | Описание |
|---|---|
| `condition_id` (PK, string) | стабильный ID строки |
| `owner_type` (enum, см. ниже) | домен-владелец |
| `owner_id` (string) | ID владельца в домене |
| `condition_type` (string) | тип условия (`level_ge`, `time_of_day`, …) |
| `params` (string) | параметры условия |
| `combine` (enum all\|any, default all) | AND или OR внутри владельца |

*Заметки: типы: `level_ge`, `time_of_day`, `weather_is`, `friendship_ge`, `has_item`, `quest_done`, `species_defeated`, `ability_used_n_times`, `species_in_set`, … **новый WIP = новый тип, не миграция**.*

**`effect`** — универсальные эффекты для любых владельцев

| Поле | Описание |
|---|---|
| `effect_id` (PK, string) | стабильный ID строки |
| `owner_type` (enum, см. ниже) | домен-владелец |
| `owner_id` (string) | ID владельца в домене |
| `effect_type` (string) | тип эффекта (`damage`, `apply_status`, …) |
| `target` (string) | цель эффекта |
| `params` (string) | параметры эффекта |
| `chance` (float) | шанс срабатывания, 0..1 |
| `duration` (uint?) | длительность, если применимо |

*Заметки: типы: `damage` (формула C++), `apply_status`, `modify_stat`, `buff`, `teleport`, `give_item`, `trigger_evolution`, `capture_bonus`, `equip_slot`, `give_achievement`, …*

`owner_type` — закрытый список доменов (= имя таблицы: `species`, `species_ability`, `evolution`,
`ability`, `breeding`, `item`, `status`, `mutation`, `weather`, `zone_spawn`, `zone_encounter`,
`shop_stock`, `dialogue`, `dialogue_node`, `quest`, `craft_recipe`, `achievement`,
`trainer_rules`, `event`, …); валидатор проверяет, что `owner_id` существует в указанном домене.
Правила зон и типа зоны (`zone`/`zone_type`) — отложены (`99-open-questions` #10).

---

## 4. Runtime-слой (SaveGame — не «БД», но контракт)

| Сущность | Состав |
|---|---|
| `player_profile` | `display_name`, `appearance[] (option_id)`, `card_tiles[]` |
| `player_currency` | `(currency_id, balance)` — список балансов |
| `inventory` | `(item_id, count)` — стеки; квестовые помечаются категорией шаблона; банк = отдельный список, партия = слоты 0–5 |
| `creature_instance` | `uuid (PK)`, `species_id`, `nickname?`*, `level`, `xp`, `current_hp`, `stats` (rolled base + trained + боевые модификаторы), `stat_training_points`, `ability_slots[≤5]` + **счётчики использования** (для требований изучения), `nature_id`, `gender`, `friendship`, `size_value`, `pigment_hex`, `mutation_ids[]`, `cosmetics[] (slot→item)`, `status[]` (персистентные до лечения — GDD), `species_art_id?`, `origin (wild\|bred\|legendary\|event)`, `met_location_id`, `met_date` |
| `pokedex_state` | `(species_id, seen, caught)` |
| `quest_progress` | `(quest_id, objective_seq, counters[])` |
| `achievement_progress` | `(achievement_id, value, done)` |
| `world_state` | время суток, `spawned[zone→instance ids]`, флаги легендарных, override погоды |
| `save_meta` | `schema_version`, `balance_version`, `game_version`, `saved_at`, `playtime_s` — **миграции по версиям (№12)** |

`*` nickname в GDD отсутствует — колонка зарезервирована, не используется до расширения GDD.

## 5. Принятые решения

| # | Тема | Решение |
|---|---|---|
| 1 | ID-схема | namespaced slug (`species.bulbasaur`), синтаксис = GameplayTags |
| 2 | Условия/эффекты | построчные `Rules/condition` + `Rules/effect` (AND; «ИЛИ» = несколько владельцев) |
| 3 | Матрица типов | полная сетка 12×12 = 144 строки |
| 4 | Параметры | частые — типизированные колонки; редкие — `params`-строка |
| 5 | Локализация | отдельный слой `Meta/locale` |
| 6 | Зоны | конфиг в CSV + volume-актор в уровне (`zone_id`), границ не в CSV |
| 7 | Species | хаб + child-таблицы; `Species/Mutations/` вложены (Pigments отменены — №19) |
| 8 | `Data/` | база разработчика + домены Battle/World/Activities/Rules/Meta |
| 9 | `Content/` | база разработчика + `Input/`, `Caves→Instances`, `Pokemon/Base` |
| 10 | Термин | **`Ability`** (`Data/Abilities/`), рус. «способность» — термин GDD |
| 11 | Формулы | база в C++ (тесты), параметры/веса в CSV, кривые — CurveTable |
| 12 | Версии save | `schema_version` + `balance_version`, явные C++-миграции, автопересчёт только по правилу |
| 13 | Чужие механики | держимые предметы, яйца, бейджи, фракции, `unlocked_zones`, боксы — **не делаем**; яйца → открытые вопросы |
| 14 | Нейминг данных | таблицы — ед. ч.; квалификатор после существительного (`catch_rate_base`, `size_min`); bool — `is_`/`has_`; время — `_seconds`; PK = `<entity>_id` (AGENTS §5) |
| 15 | Типы и дефолты | `uint` = неотрицательное; `(uniq)` = уникальная комбинация вне PK; пустая ячейка CSV = `default` |
| 16 | Гибкость без jsonb-колонок | два канала: правила игры → `Rules/condition` + `Rules/effect` + `reward` (`owner_type`/`owner_id`); конфиги активностей → колонка `config`/`rules` (путь к файлу) |
| 17 | Поведение | `species.behavior` + `behavior_mod`; зона переопределяет (`zone_spawn.override_behavior`); влияние характеров отложено |
| 18 | Статы | `species_stats` широкой (6 баз + 6 весов), EAV отменён; пул очков — `Balance` (ресерч Pokémon: база фиксирована, IV/EV) |
| 19 | Пигменты | `tint_variance` — коэффициент от `tint_base`; `species_pigment` удалена |
| 20 | Рецепты | `craft_recipe` — указатель на `rules`-файл (inputs/outputs, много результатов); `recipe_ingredient` удалена |
| 21 | Диалоги | граф через `prev_node_id` + `sort_order`; варианты ответов = дочерние ноды |
| 22 | Активности | тонкие таблицы: `kind` (выбирает код-логику) + `config` (путь к файлу) |

## 6. Утверждённые каталоги

```text
Data/
├── Species/            # species, species_stats, species_ability, evolution,
│   │                   #   species_cosmetic, species_food, species_breeding,
│   │                   #   species_breeding_group, species_art
│   └── Mutations/      # mutation, species_mutation
├── Types/              # type, type_chart
├── Abilities/          # ability          (эффекты/условия → Rules/)
├── Battle/             # status, nature
├── Items/              # item, craft_recipe (+ файлы rules-рецептов)
├── Zones/              # zone, zone_spawn, zone_encounter, zone_weather, weather
├── World/              # time_of_day
├── NPCs/               # npc, shop, dialogue, npc_dialogue, dialogue_node, shop_stock,
│                       #   battle_group, battle_group_member, trainer_rules
├── Quests/             # quest, quest_objective, reward
├── Activities/         # tournament_config, roguelike_cave_config, contest_config,
│                       #   minigame, achievement, event
├── Rules/              # condition, effect
├── Balance/            # gameplay_config
└── Meta/               # locale, currency, appearance_option, cosmetic_slot

Content/
├── Core/               # GameMode, PlayerController, GameState, PlayerState (тонкие BP)
├── Input/              # Input Actions, Mapping Contexts (со Слоя 2)
├── Characters/         # BP_Player, BP_NPC_Base, анимации
├── Pokemon/
│   ├── Base/           # BP_Pokemon_Base, общий скелет/анимации
│   ├── Species/<species_id>/   # уникальные ассеты вида (папка по необходимости)
│   └── Mutations/      # меши/материалы мутаций
├── World/
│   ├── Overworld/      # бесшовная карта
│   └── Instances/      # изолированные зоны (пещеры, интерьеры, спец-зоны)
├── UI/
├── VFX/
└── Audio/
```

Именование ассетов по Epic (`BP_`, `WBP_`, `SM_`, `SK_`, `M_`, `T_`, `S_`, `A_`) —
фиксация на слое «Дизайн».

## 7. Зарезервированные расширения (WIP-слоты)

| WIP из GDD | Где живёт, когда созреет |
|---|---|
| Мега-эволюции / первобытная реверсия | новые строки/таблица рядом с `species_art` + condition/trigger-типы |
| Награды генеративных турниров | строки `reward` (owner=tournament_config) |
| Мини-игры (реакционные, крафт) | `minigame`, `config`-файлы конкурсов и рецептов — структура готова, контент WIP |
| Катсцены | уровень контента (Content/), данные — при появлении ТЗ |
| Формат размножения (яйца?) | открытый вопрос `Docs/GDD/99-open-questions.md` #10 |

Отложенные и открытые вопросы ревью схемы — `Docs/data/99-open-questions.md`.

## 8. Комментарии на ревью

Редактируй таблицу прямо в этом файле (или комментируй в PR ветки `docs/layer1-data-schema`):
комментарий к полю — указывай **таблицу и имя поля** (например, `ability.power`).

| # | Комментарий / правка | Статус |
|---|---|---|
| | | |
