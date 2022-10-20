use tempeh_miniaudio::engine::Engine;

fn main() {
    let mut engine = Engine::new().unwrap();
    engine.play_sound();

    println!("Press anything to stop");
    let mut line = String::new();
    std::io::stdin().read_line(&mut line).unwrap();
}