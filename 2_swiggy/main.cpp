#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
// 1. catalogue
// 1.1 item
// 1.2 catalogue manager
// 1.2.1 CRUD item, given restaurant
// 1.2.2 Read items grouped by restaurant

// TODO: encapsulation
struct Location
{
    // address, coordinates
    std::string address;
    std::pair<float, float> latlong{0.f, 0.f};
};

// TODO: encapsulation
struct Restaurant
{
    // id, name, location
    int id{0};
    std::string name;
    Location location;
    std::string pwd;
};

// TODO: encapsulation
struct Item
{
    // id, name, picture, price, available quantity
    int id{0};
    std::string name;
    std::string url;
    float price;
    int availability{0};
    int restaurant_id;

    Item(int i, const std::string &n, const std::string &u, float p, int a, int r) : id(i), name(n), url(u), price(p), availability(a), restaurant_id(r)
    {
    }
};

// TODO: encapsulation
struct Catalogue
{
    // items
    std::vector<Item> items;

    int add(Item i)
    {
        items.push_back(i);
        return i.id;
    }

    int remove(int id)
    {
        // find the item
        // remove
        return id;
    }

    int update(int id, Item i)
    {
        // find the item
        // replace
        return id;
    }

    std::optional<Item> get(int id) const
    {
        auto iter = std::find(items.begin(), items.end(), [&](const auto &i)
                              { return i.id == id; });
        if (iter != items.end())
        {
            return *iter;
        }
        return {};
    }
};

struct IdGenerator
{
    enum type
    {
        ITEM,
        RESTAURANT,
        ORDER,
        USER,
        PAYMENTMETHOD,
    };
    static std::unordered_map<type, int> counters;
    static int generate(type t)
    {
        return ++counters[t];
    }
};

// TODO: encapsulation
struct CatalogueService
{
    Catalogue catalogue;
    // read catalogue items grouped by restaurants
    // crud item
    int add_item(Item i)
    {
        return catalogue.add(i);
    }
    int remove_item(int id)
    {
        return catalogue.remove(id);
    }
    int update_item(int id, Item i)
    {
        return catalogue.update(id, std::move(i));
    }
    std::optional<Item> get_item(int id) const
    {
        return catalogue.get(id);
    }
    std::unordered_map<int, std::vector<Item>> items_group_by_restaurant()
    {
        return {};
    }
    static CatalogueService &get_instance()
    {
        static CatalogueService cs;
        return cs;
    }
};

// ------------------------------------------------------------------------------------------
// Users
// id, name

struct User
{
    int id{0};
    std::string name;
    std::string pwd;
    User(std::string n, std::string p) : name(n), pwd(p) {}
};

struct UserService
{
    static UserService &get_instance()
    {
        static UserService us;
        return us;
    }
    std::vector<User> users;
    std::unordered_map<int, std::vector<PaymentMethod>> payment_methods;
    std::unordered_map<int, int> default_payment_method_id;
    int create_user(std::string name, std::string pwd)
    {
        users.push_back(User(std::move(name), std::move(pwd)));
    }

    std::optional<User> get_user(std::string name, std::string pwd) const
    {
        auto iter = std::find(users.begin(), users.end(), [&](const auto &u)
                              { return u.name == name && u.pwd == pwd; });
        if (iter != users.end())
        {
            return *iter;
        }
        return {};
    }

    void add_payment_method(int user_id, PaymentMethod &method)
    {
        payment_methods[user_id].push_back(method);
    }

    std::vector<PaymentMethod> get_payment_methods(int user_id) const
    {
        return payment_methods.at(user_id);
    }

    void set_default_payment_method(int user_id, int payment_method_id)
    {
        default_payment_method_id[user_id] = payment_method_id;
    }

    PaymentMethod get_default_payment_method(int user_id) const
    {
        const auto &pms = payment_methods.at(user_id);
        const auto &default_method_id = default_payment_method_id.at(user_id);
        auto iter = std::find(pms.begin(), pms.end(), [&](const auto &pm)
                              { return pm.id == default_method_id; });
        if (iter != pms.end())
        {
            return *iter;
        }
        return pms[0];
    }
};

// ------------------------------------------------------------------------------------------

// Basket
// items, quantities

struct Basket
{
    std::unordered_map<int, int> items; // item id, item quantity
    // TODO: add constraints based on item availability
    void add(int id, int quantity)
    {
        items[id] += quantity;
    }

    void increase(int id)
    {
        add(id, 1);
    }

    void decrease(int id)
    {
        add(id, -1);
    }

    void remove(int id)
    {
        items.erase(id);
    }
};

// ------------------------------------------------------------------------------------------
struct PaymentMethod
{
    int id{0};
    virtual bool credit(int amount) = 0;
    virtual bool debit(int amount) = 0;
};
struct UPI : PaymentMethod
{
    UPI()
    {
        id = IdGenerator::generate(IdGenerator::type::PAYMENTMETHOD);
    }
    std::string upi_id;
    bool credit(int amount) override
    {
        return true;
    }
    bool debit(int amount) override
    {
        return true;
    }
};

struct Card : PaymentMethod
{
    Card()
    {
        id = IdGenerator::generate(IdGenerator::type::PAYMENTMETHOD);
    }
    std::string card_no;
    int cvv;
    std::string name;
    bool credit(int amount) override
    {
        return true;
    }
    bool debit(int amount) override
    {
        return true;
    }
};

struct PaymentService
{
    static PaymentService &get_instance()
    {
        static PaymentService p;
        return p;
    }

    bool pay(int amount, PaymentMethod &&from, PaymentMethod &&to)
    {
        // TODO make this atomic
        from.debit(amount);
        to.credit(amount);
        return true;
    }
};

// ------------------------------------------------------------------------------------------
struct RestaurantAccountService
{
    std::vector<Restaurant> restaurants;
    std::unordered_map<int, std::vector<PaymentMethod>> payment_methods;
    std::unordered_map<int, int> default_payment_method_id;
    static RestaurantAccountService &get_instance()
    {
        static RestaurantAccountService rs;
        return rs;
    }
    bool creat_account(Restaurant r)
    {
        restaurants.push_back(r);
    }
    void add_payment_method(int rest_id, PaymentMethod &p)
    {
    }

    void set_default_payment_method(int rest_id, int payment_method_id)
    {
        default_payment_method_id[rest_id] = payment_method_id;
    }

    PaymentMethod get_default_payment_method(int rest_id) const
    {
        const auto &pms = payment_methods.at(rest_id);
        const auto &default_method_id = default_payment_method_id.at(rest_id);
        auto iter = std::find(pms.begin(), pms.end(), [&](const auto &pm)
                              { return pm.id == default_method_id; });
        if (iter != pms.end())
        {
            return *iter;
        }
        return pms[0];
    }
};

struct RestaurantService
{
    static RestaurantService &get_instance()
    {
        static RestaurantService rs;
        return rs;
    }

    // crud on catalog items
    void add_item(Item i)
    {
        auto &cs = CatalogueService::get_instance();
        cs.add_item(i);
    }
};

// ------------------------------------------------------------------------------------------

struct Order
{
    int id{0};
    int payment_method_id;
    int user_id;
    std::vector<std::pair<Item, int>> items;
    int time;

    Order(int p, int u, std::vector<std::pair<Item, int>> i, int t) : payment_method_id(p), user_id(u), items(std::move(i)), time(t)
    {
        id = IdGenerator::generate(IdGenerator::type::ORDER);
    }
};

struct OrderService
{
    std::vector<Order> orders;
    bool place_order(std::vector<std::pair<Item, int>> items, int user_id, int payment_method_id)
    {

        const auto &user_service = UserService::get_instance();
        const auto &restaurant_service = RestaurantAccountService::get_instance();
        std::unordered_map<int, int> total_cost;
        for (const auto &[item, quantity] : items)
        {
            total_cost[item.restaurant_id] += item.price * quantity;
        }
        for (const auto &[rest_id, cost] : total_cost)
        {
            PaymentService::get_instance()
                .pay(cost, user_service.get_default_payment_method(user_id), restaurant_service.get_default_payment_method(rest_id));
        }
        orders.push_back(Order(payment_method_id, user_id, items, 1)); // TODO put time here.
    }

    std::vector<Order> get_orders_for_restaurant(int rest_id)
    {
        std::vector<Order> res;
        for (const auto &o : orders)
        {
            if (o.res)
        }
        return res;
    }
};
// ------------------------------------------------------------------------------------------

struct CustomerService
{
    std::unordered_map<int, Basket> customer_basket;

    static CustomerService &get_instance()
    {
        static CustomerService cs;
        return cs;
    }
    std::unordered_map<int, std::vector<Item>> get_items_grouped_by_restaurant()
    {
        return CatalogueService::get_instance().items_group_by_restaurant();
    }

    // add items to basket
    void add_to_basket(int user_id, int item_id, int quantity)
    {
        customer_basket[user_id].add(item_id, quantity);
    }

    void remove_from_basket(int user_id, int item_id)
    {
        customer_basket[user_id].remove(item_id);
    }

    void increase_quantity(int user_id, int item_id)
    {
        customer_basket[user_id].increase(item_id);
    }

    void decrease_quantity(int user_id, int item_id)
    {
        customer_basket[user_id].decrease(item_id);
    }

    std::vector<std::pair<Item, int>> get_basket_items(int user_id)
    {
        const auto &cat = CatalogueService::get_instance();
        std::vector<std::pair<Item, int>> res;
        for (const auto &[item_id, quantity] : customer_basket[user_id].items)
        {
            if (const auto i = cat.get_item(item_id))
                res.push_back({*i, quantity});
        }
        return res;
    }
};
