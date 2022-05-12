if (typeof db == 'undefined') var db = {};
db = {

cache: null,

MAX_TWEET_CACHE_SIZE: 4096,

MAX_USER_CACHE_SIZE: 1024,

version: 3,

init:
function init (callback) {
    db.get_version(function (version) {
        var db_version = parseInt(version);
        if (db_version === 2) { // from 2 to 3
            db.create_cache(function () {
                db.update_version(callback);
            });
        } else if (db_version === db.version) {
            if (typeof (callback) != 'undefined') {
                callback();
            }
        } else { // rebuild
            db.create_sys(function () {
                db.create_cache(function () {
                    db.update_version(callback);
                });
            });
        }
    });
},

create_sys:
function create_sys(callback) {
    for (var x in window.localStorage) {
        if (x.match(/^info\./)) {
            window.localStorage.removeItem(x);
        }
    }
    $(window).dequeue('_database');

    for (var x in window.localStorage) {
        if (x.match(/^profile\./)) {
            window.localStorage.removeItem(x);
        }
    }
    $(window).dequeue('_database');

    window.localStorage["info.settings"] = JSON.stringify(conf.default_settings),
    $(window).dequeue('_database');

    if (typeof (callback) != 'undefined') {
        callback();
    }

    $(window).queue('_database', procs);
    $(window).dequeue('_database');
},

update_version:
function update_version(callback) {
    window.localStorage["info.version"] = db.version;
    $(window).dequeue('_database');

    if (typeof (callback) != 'undefined') {
        callback();
    }

    $(window).queue('_database', procs);
    $(window).dequeue('_database');
},

create_cache:
function create_cache(callback) {
    for (var x in window.localStorage) {
        if (x.match(/^tweet_cache\./)) {
            window.localStorage.removeItem(x);
        }
    }
    $(window).dequeue('_database');

    for (var x in window.localStorage) {
        if (x.match(/^user_cache\./)) {
            window.localStorage.removeItem(x);
        }
    }
    $(window).dequeue('_database');

    if (typeof (callback) != 'undefined') {
        callback();
    }

    $(window).queue('_database', procs);
    $(window).dequeue('_database');
},

dump_users:
function dump_users(json_obj) {
    var dump_single_user = function (tx, user) {
        // update user obj
        window.localStorage[`user_cache.${user.id_str}`] = JSON.stringify(user);
    };
    // dump users
    db.database.transaction(function (tx) {
        for (var i = 0, l = json_obj.length; i < l; i += 1) {
            var user = json_obj[i];
            dump_single_user(tx, user);
        }
    });
},

dump_tweets:
function dump_tweets(json_obj) {
    var dump_single_user = function (tx, user) {
        // update user obj
        window.localStorage[`user_cache.${user.id_str}`] = JSON.stringify(user);
    };
    var dump_single_tweet = function (tx, tweet_obj) {
        window.localStorage[`tweet_cache.${tweet_obj.id_str}`] = JSON.stringify(tweet_obj);
    };

    // dump tweets
    db.database.transaction(function (tx) {
        for (var i = 0, l = json_obj.length; i < l; i += 1) {
            var tweet_obj = json_obj[i];
            if (tweet_obj.hasOwnProperty('retweeted_status')) {
                dump_single_tweet(tx, tweet_obj['retweeted_status']);
            }
            dump_single_tweet(tx, tweet_obj);
        }
    });
    // dump users
    db.database.transaction(function (tx) {
        for (var i = 0, l = json_obj.length; i < l; i += 1) {
            var tweet_obj = json_obj[i];
            var user = typeof tweet_obj.user != 'undefined'
                ? tweet_obj.user: tweet_obj.sender;
            dump_single_user(tx, user);
        }
    });
},

get_version:
function get_version(callback) {
    const val = window.localStorage["info.version"];

    if (val == null) {
        callback(-1);
        return;
    }

    callback(parseInt(val));
},


get_tweet:
function get_tweet(key, callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT id, status, json FROM TweetCache WHERE id=?', [key], 
            function(tx, rs) {callback(tx,rs);});
    });
},

get_user:
function get_user(screen_name, callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT id, screen_name, json FROM UserCache WHERE screen_name=?', [screen_name], 
        function (tx, rs) {
            if (rs.rows.length != 0) {
                callback(JSON.parse(rs.rows.item(0).json));
            } else {
                callback(null);
            }
        });
    });
},

search_user:
function search_user(query, callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT user_id, screen_name, json FROM UserCache WHERE screen_name LIKE \'%'+query+'%\'', [], 
            function(tx, rs) {callback(tx,rs);});
    });
},

get_screen_names_starts_with:
function get_users_starts_with(starts, callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT screen_name FROM UserCache WHERE screen_name LIKE \''+starts+'%\'', [], 
            function(tx, rs) {callback(tx,rs);});
    });
},

get_screen_names:
function get_screen_names(callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT screen_name FROM UserCache ORDER BY screen_name', [], 
            function(tx, rs) {callback(tx,rs);});
    });
},

reduce_user_cache:
function reduce_user_cache(limit, callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('DELETE FROM UserCache WHERE id in (SELECT id FROM UserCache ORDER BY id limit ?)', [limit], callback);
    });
},

reduce_tweet_cache:
function reduce_tweet_cache(limit, callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('DELETE FROM TweetCache WHERE id in (SELECT id FROM TweetCache ORDER BY id limit ?)', [limit], callback);
    });
},

get_tweet_cache_size:
function get_tweet_cache_size(callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT count(*) FROM TweetCache', [],
        function (tx, rs) {
            callback(rs.rows.item(0)['count(*)']);
        });
    });
},

get_user_cache_size:
function get_user_cache_size(callback) {
    db.database.transaction(function (tx) {
        tx.executeSql('SELECT count(*) FROM UserCache', [],
        function (tx, rs) {
            callback(rs.rows.item(0)['count(*)']);
        });
    });
},

reduce_db:
function reduce_db () {
    db.get_tweet_cache_size(function (size) {
        if (db.MAX_TWEET_CACHE_SIZE < size) {
            db.reduce_tweet_cache(
                parseInt(db.MAX_TWEET_CACHE_SIZE*2/3)
            , function () {
            })
        }
    });
    db.get_user_cache_size(function (size) {
        if (db.MAX_USER_CACHE_SIZE < size) {
            db.reduce_user_cache(
                parseInt(db.MAX_USER_CACHE_SIZE*2/3)
            , function () {
            })
        }
    });
},

save_option:
function save_option(key, value, callback) {
    window.localStorage[`info.${key}`] = value;
    callback(true);
},

load_option:
function load_option(key, callback) {
    const val = window.localStorage[`info.${key}`];

    if (val == null) {
        callback(null);
        return;
    }

    callback(val);
},

save_profile_prefs:
function save_profile_prefs(name, json, callback) {
    const val = window.localStorage[`profile.${name}`];
    var prof = JSON.parse(val);
    prof.preferences = json;
    window.localStorage[`profile.${name}`] = JSON.stringify(prof);
    callback(true);
},

load_profile_prefs:
function load_profile_prefs(name, callback) {
    const val = window.localStorage[`profile.${name}`];

    if (val == null) {
        callback('{}');
        return;
    }

    var prof = JSON.parse(json);
    callback(prof.preferences);
},

add_profile:
function add_profile(prefix, protocol, callback) {
    const name = prefix+'@'+protocol;
    window.localStorage[`profile.${name}`] = JSON.stringify({
        "name":        name,
        "protocol":    protocol,
        "preferences": JSON.stringify(conf.get_default_prefs(protocol)),
        "order":       0,
    });
    callback(true);
},

remove_profile:
function remove_profile(name, callback) {
    window.localStorage.removeItem(`profile.${name}`);
    callback(true);
},

modify_profile:
function modify_profile(name, profile, callback) {
    window.localStorage[`profile.${name}`] = JSON.stringify({
        "name":        profile.name,
        "protocol":    profile.protocol,
        "preferences": profile.preferences,
        "order":       profile.order,
    });
    callback(true);
},

get_profile:
function get_profile(name, callback) {
    const val = window.localStorage[`profile.${name}`];

    if (val == null) {
        callback({});
        return;
    }

    var prof = JSON.parse(json);
    callback(prof);
},

get_all_profiles:
function get_all_profiles(callback) {
    var profs = [];

    for (var x in window.localStorage) {
        if (x.match(/^profile\./)) {
            console.log(x);
            profs.push(JSON.parse(window.localStorage[x]));
        }
    }
    callback(profs);
}

};

