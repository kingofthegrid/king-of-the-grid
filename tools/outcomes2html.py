import json
import sys
from datetime import datetime


OUTCOMES_JSON = sys.argv[1]
OUTPUT_HTML = sys.argv[2]
PREFIX_PATH = sys.argv[3]

# Read the JSON data from outcome.json
with open(OUTCOMES_JSON, "r") as file:
    data = json.load(file)

seed = data["seed"]
date_s = str(datetime.now())

# Generate the HTML content
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>King Of The Grid Leaderboard</title>
    <link rel="stylesheet" href="bootstrap.min.css">
    <link rel="stylesheet" href="bootstrap-icons.min.css">
    <script src="jquery-3.7.1.min.js"></script>
    <script src="bootstrap.bundle.min.js"></script>
    <style>
        .tab-content {{
            border-left: 1px solid #444;
            border-right: 1px solid #444;
            border-bottom: 1px solid #444;
            padding: 10px;
        }}
        
        .table {{
            margin-bottom: 0;
        }}

        .nav-tabs {{
            margin-bottom: 0;
        }}
    </style>
</head>
<body>
    <div class="container mt-5">
        <div id="game-header" class="d-flex justify-content-center align-items-center">
            <div class="card mb-3">
                <div class="row no-gutters">
                    <div class="col-md-3 text-center align-middle">
                        <img src="kotg.png" width="128px" class="mb-4 mt-4">
                    </div>
                    <div class="col-md-9">
                        <div class="card-body">
                            <h5 class="card-title">King Of The Grid</h5>
                            <p class="card-text">A Sandbox Competition Game in which developers can write bots to compete
                                on the most efficient algorithm to be King Of The Grid!</p>
                            <p class="card-text">
                                Process is simple:
                                <ul>
                                    <li>Evaluate <a href="https://github.com/kingofthegrid/king-of-the-grid/blob/master/src/rules.cpp">game rules</a></li>
                                    <li>Write your bot implementation <a href="ide/">in an online IDE</a></li>
                                    <li>Test your bot against others inside <a href="run.html">the runner</a></li>
                                    <li>Submit your bot <a href="https://github.com/kingofthegrid/bots">here</a></li>
                                </ul>
                            </p>
                            <a href="ide/" class="btn btn-outline-info mr-2 mb-2" role="button"><i class="bi bi-terminal"></i> Write Your Bot In-Browser</a>
                            <a href="run.html" class="btn btn-outline-light mr-2 mb-2" role="button"><i class="bi bi-robot"></i> Test Your Bots In-Browser</a>
                            <a href="https://github.com/kingofthegrid/bots" class="btn btn-outline-warning mr-2 mb-2" role="button"><i class="bi bi-chat-square-dots-fill"></i> Submit Your Bot</a>
                            <a href="https://github.com/kingofthegrid/king-of-the-grid" class="btn btn-outline-light mb-2"><i class="bi bi-github"></i> Source</a>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <ul class="nav nav-tabs">
            <li class="nav-item">
                <a class="nav-link active" data-toggle="tab" href="#leaderboard">Leaderboard ({date_s}) seed ({seed})</a>
            </li>
            <li class="nav-item">
                <a class="nav-link" data-toggle="tab" href="#rules">Game Rules</a>
            </li>
            <li class="nav-item">
                <a class="nav-link" data-toggle="tab" href="#how-to-build">How To Build</a>
            </li>
        </ul>
        <div id="myTabContent" class="tab-content">
            <div class="tab-pane fade active show" id="leaderboard">
                <table class="table">
                    <tr class="table-secondary">
                        <th>Bot Name</th>
                        <th>Score</th>
                        <th>Wins</th>
                        <th>Draws</th>
                    </tr>
"""

# Generate table rows from the JSON data
for index, entry in enumerate(data["results"]):
    bot_name = entry["bot"]
    score = entry["score"]
    wins = entry["wins"]
    draws = entry["draws"]

    win_links = ", ".join([f'<a href="browser.html#{PREFIX_PATH}{win}" target="_blank">{idx}</a>' for idx, win in enumerate(wins, 1)])
    draw_links = ", ".join([f'<a href="browser.html#{PREFIX_PATH}{draw}" target="_blank">{idx}</a>' for idx, draw in enumerate(draws, 1)])

    html_content += f"""
                    <tr class="table-dark">
                        <td>{index+1}. {bot_name}</td>
                        <td>{score}</td>
                        <td>{win_links if wins else "No Wins"}</td>
                        <td>{draw_links if draws else "No Draws"}</td>
                    </tr>
    """

# Close the HTML tags
html_content += """
                </table>
            </div>
            <div class="tab-pane fade" id="rules">
                <h3 class="mt-3 ml-3">Game Rules</h3>
                <ul>
                    <li>See <a href="https://github.com/kingofthegrid/king-of-the-grid/blob/master/src/rules.cpp">rules.cpp</a> for exact rules (how often, what values etc).<br/></li>
                    <li>World size: 32x32 cells</li>
                    <li>Two bots (or programs) spawn in top-left and bottom-right corners</li>
                    <li>Each bot has a set of energy. See rules.cpp for exact values.</li>
                    <li>Bots run on Z80 CPU. See CPUs section.</li>
                    <li>Food (static) and prey (moves) are spawned periodically.</li>
                    <li>Food is not spawned evenly: random noise is applied using FastNoiseLite on rules described on seasons.cpp</li>
                    <li>Both can move around, eat, scan environment and clone themselves. See bot_api.h</li>
                    <li>Bot that survives the longest, wins.</li>
                    <li>If both bots survive for too long, it's a draw.</li>
                </ul>
            </div>
            <div class="tab-pane fade" id="how-to-build">
                <a href="ide/" class="btn btn-outline-info mr-2" role="button"><i class="bi bi-terminal"></i> Build your bot in an Online IDE</a>
                <a href="https://github.com/kingofthegrid/king-of-the-grid?tab=readme-ov-file#how-to-write-your-bot">See more details here.</a>
            </div>
        </div>
    </div>
</body>
</html>
"""

# Save the HTML content to a file
with open(OUTPUT_HTML, "w") as file:
    file.write(html_content)

print("HTML page generated successfully as 'outcome.html'.")
