#!/usr/bin/env python3
"""Study Planner Web Frontend - HTTP API Server"""

import http.server
import json
import sqlite3
import os
import urllib.parse
from pathlib import Path

DB_PATH = os.environ.get("STUDY_DB", "../study_planner.db")
FRONTEND_DIR = Path(__file__).parent


def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


def dict_from_row(row):
    if row is None:
        return None
    return dict(row)


def rows_to_list(rows):
    return [dict(r) for r in rows]


class APIHandler(http.server.BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass  # suppress logs

    def send_json(self, data, status=200):
        body = json.dumps(data, default=str).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def send_error_json(self, msg, status=400):
        self.send_json({"error": msg}, status)

    def read_body(self):
        length = int(self.headers.get("Content-Length", 0))
        if length == 0:
            return {}
        return json.loads(self.rfile.read(length))

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        params = urllib.parse.parse_qs(parsed.query)

        try:
            db = get_db()
            if path == "/" or path == "/index.html":
                self.serve_frontend()
            elif path == "/api/dashboard":
                self.api_dashboard(db)
            elif path == "/api/subjects":
                self.api_list_subjects(db)
            elif path == "/api/tasks":
                self.api_list_tasks(db, params)
            elif path == "/api/sessions":
                self.api_list_sessions(db, params)
            elif path == "/api/plans":
                self.api_list_plans(db, params)
            elif path == "/api/plans/progress":
                self.api_plan_progress(db, params)
            elif path == "/api/analytics/by-subject":
                self.api_analytics_by_subject(db, params)
            elif path == "/api/analytics/daily":
                self.api_analytics_daily(db, params)
            elif path == "/api/study-sets":
                self.api_list_study_sets(db)
            else:
                self.send_error_json("Not found", 404)
            db.close()
        except Exception as e:
            self.send_error_json(str(e), 500)

    def do_POST(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        body = self.read_body()

        try:
            db = get_db()
            if path == "/api/subjects":
                self.api_create_subject(db, body)
            elif path == "/api/tasks":
                self.api_create_task(db, body)
            elif path == "/api/sessions":
                self.api_create_session(db, body)
            elif path == "/api/plans":
                self.api_create_plan(db, body)
            elif path == "/api/study-sets":
                self.api_create_study_set(db, body)
            else:
                self.send_error_json("Not found", 404)
            db.close()
        except Exception as e:
            self.send_error_json(str(e), 500)

    def do_PUT(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        body = self.read_body()

        try:
            db = get_db()
            if path.startswith("/api/tasks/") and path.endswith("/status"):
                task_id = int(path.split("/")[3])
                self.api_update_task_status(db, task_id, body)
            elif path.startswith("/api/sessions/") and path.endswith("/stop"):
                session_id = int(path.split("/")[3])
                self.api_stop_session(db, session_id)
            elif path.startswith("/api/plans/") and path.endswith("/complete"):
                plan_id = int(path.split("/")[3])
                self.api_complete_plan(db, plan_id)
            else:
                self.send_error_json("Not found", 404)
            db.close()
        except Exception as e:
            self.send_error_json(str(e), 500)

    def do_DELETE(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        try:
            db = get_db()
            parts = path.split("/")
            if path.startswith("/api/subjects/") and len(parts) == 4:
                self.api_delete_subject(db, int(parts[3]))
            elif path.startswith("/api/tasks/") and len(parts) == 4:
                self.api_delete_task(db, int(parts[3]))
            elif path.startswith("/api/sessions/") and len(parts) == 4:
                self.api_delete_session(db, int(parts[3]))
            elif path.startswith("/api/plans/") and len(parts) == 4:
                self.api_delete_plan(db, int(parts[3]))
            else:
                self.send_error_json("Not found", 404)
            db.close()
        except Exception as e:
            self.send_error_json(str(e), 500)

    def serve_frontend(self):
        html_path = FRONTEND_DIR / "index.html"
        if html_path.exists():
            content = html_path.read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(content)))
            self.end_headers()
            self.wfile.write(content)
        else:
            self.send_error_json("Frontend not found", 404)

    # ---- API Methods ----

    def api_dashboard(self, db):
        cur = db.execute("SELECT COUNT(*) FROM subjects")
        subject_count = cur.fetchone()[0]
        cur = db.execute("SELECT COUNT(*) FROM tasks WHERE status != 'completed'")
        active_tasks = cur.fetchone()[0]
        cur = db.execute("SELECT COUNT(*) FROM tasks WHERE status = 'completed'")
        completed_tasks = cur.fetchone()[0]
        cur = db.execute("SELECT COALESCE(SUM(duration_seconds), 0) FROM study_sessions WHERE end_time IS NOT NULL")
        total_secs = cur.fetchone()[0]
        cur = db.execute("SELECT COALESCE(SUM(duration_seconds), 0) FROM study_sessions WHERE date(start_time) = date('now') AND end_time IS NOT NULL")
        today_secs = cur.fetchone()[0]
        cur = db.execute("SELECT id, subject_id, start_time FROM study_sessions WHERE end_time IS NULL LIMIT 1")
        running = dict_from_row(cur.fetchone())

        self.send_json({
            "subject_count": subject_count,
            "active_tasks": active_tasks,
            "completed_tasks": completed_tasks,
            "total_seconds": total_secs,
            "today_seconds": today_secs,
            "running_session": running
        })

    def api_list_subjects(self, db):
        rows = db.execute("SELECT * FROM subjects ORDER BY name").fetchall()
        self.send_json(rows_to_list(rows))

    def api_create_subject(self, db, body):
        db.execute("INSERT INTO subjects (name, description, color) VALUES (?, ?, ?)",
                   (body.get("name", ""), body.get("description", ""), body.get("color", "#4A90D9")))
        db.commit()
        self.send_json({"id": db.execute("SELECT last_insert_rowid()").fetchone()[0]}, 201)

    def api_delete_subject(self, db, sid):
        db.execute("DELETE FROM subjects WHERE id = ?", (sid,))
        db.commit()
        self.send_json({"ok": True})

    def api_list_tasks(self, db, params):
        status = params.get("status", [None])[0]
        subject_id = params.get("subject_id", [None])[0]
        sql = "SELECT t.*, s.name as subject_name FROM tasks t LEFT JOIN subjects s ON t.subject_id = s.id WHERE 1=1"
        args = []
        if status:
            sql += " AND t.status = ?"
            args.append(status)
        if subject_id:
            sql += " AND t.subject_id = ?"
            args.append(subject_id)
        sql += " ORDER BY t.priority DESC, t.created_at DESC"
        rows = db.execute(sql, args).fetchall()
        self.send_json(rows_to_list(rows))

    def api_create_task(self, db, body):
        db.execute(
            "INSERT INTO tasks (subject_id, title, description, priority, status, due_date, estimated_minutes) VALUES (?, ?, ?, ?, ?, ?, ?)",
            (body.get("subject_id", 0), body.get("title", ""), body.get("description", ""),
             body.get("priority", 0), "todo", body.get("due_date"), body.get("estimated_minutes", 0)))
        db.commit()
        self.send_json({"id": db.execute("SELECT last_insert_rowid()").fetchone()[0]}, 201)

    def api_update_task_status(self, db, tid, body):
        new_status = body.get("status", "todo")
        if new_status == "completed":
            db.execute("UPDATE tasks SET status = ?, completed_at = datetime('now') WHERE id = ?", (new_status, tid))
        else:
            db.execute("UPDATE tasks SET status = ?, completed_at = NULL WHERE id = ?", (new_status, tid))
        db.commit()
        self.send_json({"ok": True})

    def api_delete_task(self, db, tid):
        db.execute("DELETE FROM tasks WHERE id = ?", (tid,))
        db.commit()
        self.send_json({"ok": True})

    def api_list_sessions(self, db, params):
        limit = params.get("limit", [50])[0]
        rows = db.execute(
            "SELECT ss.*, s.name as subject_name FROM study_sessions ss LEFT JOIN subjects s ON ss.subject_id = s.id ORDER BY ss.start_time DESC LIMIT ?",
            (limit,)).fetchall()
        self.send_json(rows_to_list(rows))

    def api_create_session(self, db, body):
        db.execute(
            "INSERT INTO study_sessions (subject_id, task_id, start_time, notes) VALUES (?, ?, datetime('now'), ?)",
            (body.get("subject_id", 0), body.get("task_id"), body.get("notes", "")))
        db.commit()
        self.send_json({"id": db.execute("SELECT last_insert_rowid()").fetchone()[0]}, 201)

    def api_stop_session(self, db, sid):
        db.execute(
            "UPDATE study_sessions SET end_time = datetime('now'), duration_seconds = CAST((julianday('now') - julianday(start_time)) * 86400 AS INTEGER) WHERE id = ? AND end_time IS NULL",
            (sid,))
        db.commit()
        row = db.execute("SELECT duration_seconds FROM study_sessions WHERE id = ?", (sid,)).fetchone()
        self.send_json({"duration_seconds": row[0] if row else 0})

    def api_delete_session(self, db, sid):
        db.execute("DELETE FROM study_sessions WHERE id = ?", (sid,))
        db.commit()
        self.send_json({"ok": True})

    def api_list_plans(self, db, params):
        date = params.get("date", [None])[0]
        if date:
            rows = db.execute(
                "SELECT p.*, s.name as subject_name FROM plans p LEFT JOIN subjects s ON p.subject_id = s.id WHERE p.target_date = ? ORDER BY p.plan_type",
                (date,)).fetchall()
        else:
            rows = db.execute(
                "SELECT p.*, s.name as subject_name FROM plans p LEFT JOIN subjects s ON p.subject_id = s.id ORDER BY p.target_date DESC LIMIT 50"
            ).fetchall()
        self.send_json(rows_to_list(rows))

    def api_create_plan(self, db, body):
        db.execute(
            "INSERT INTO plans (subject_id, plan_type, target_date, target_minutes, description) VALUES (?, ?, ?, ?, ?)",
            (body.get("subject_id", 0), body.get("plan_type", "daily"),
             body.get("target_date", ""), body.get("target_minutes", 0),
             body.get("description", "")))
        db.commit()
        self.send_json({"id": db.execute("SELECT last_insert_rowid()").fetchone()[0]}, 201)

    def api_complete_plan(self, db, pid):
        db.execute("UPDATE plans SET is_completed = 1 WHERE id = ?", (pid,))
        db.commit()
        self.send_json({"ok": True})

    def api_delete_plan(self, db, pid):
        db.execute("DELETE FROM plans WHERE id = ?", (pid,))
        db.commit()
        self.send_json({"ok": True})

    def api_plan_progress(self, db, params):
        date = params.get("date", [None])[0]
        if not date:
            self.send_error_json("date parameter required")
            return
        rows = db.execute("""
            SELECT p.*, s.name as subject_name,
                   COALESCE((SELECT SUM(ss.duration_seconds) FROM study_sessions ss
                    WHERE ss.subject_id = p.subject_id AND date(ss.start_time) = ? AND ss.end_time IS NOT NULL), 0) as actual_seconds
            FROM plans p LEFT JOIN subjects s ON p.subject_id = s.id
            WHERE p.target_date = ?
        """, (date, date)).fetchall()
        result = []
        for r in rows_to_list(rows):
            r["actual_minutes"] = r["actual_seconds"] // 60
            r["completion_pct"] = round((r["actual_seconds"] / (r["target_minutes"] * 60)) * 100, 1) if r["target_minutes"] > 0 else 0
            result.append(r)
        self.send_json(result)

    def api_analytics_by_subject(self, db, params):
        start = params.get("start", [None])[0]
        end = params.get("end", [None])[0]
        if not start or not end:
            self.send_error_json("start and end parameters required")
            return
        rows = db.execute("""
            SELECT s.id, s.name, s.color,
                   COALESCE(SUM(ss.duration_seconds), 0) as total_seconds,
                   COUNT(ss.id) as session_count
            FROM subjects s
            LEFT JOIN study_sessions ss ON s.id = ss.subject_id
                AND ss.start_time >= ? AND ss.start_time <= ? AND ss.end_time IS NOT NULL
            GROUP BY s.id
            ORDER BY total_seconds DESC
        """, (start, end + " 23:59:59")).fetchall()
        self.send_json(rows_to_list(rows))

    def api_analytics_daily(self, db, params):
        start = params.get("start", [None])[0]
        end = params.get("end", [None])[0]
        if not start or not end:
            self.send_error_json("start and end parameters required")
            return
        rows = db.execute("""
            SELECT date(start_time) as date,
                   COALESCE(SUM(duration_seconds), 0) as total_seconds,
                   COUNT(*) as session_count
            FROM study_sessions
            WHERE start_time >= ? AND start_time <= ? AND end_time IS NOT NULL
            GROUP BY date(start_time)
            ORDER BY date
        """, (start, end + " 23:59:59")).fetchall()
        self.send_json(rows_to_list(rows))

    def api_list_study_sets(self, db):
        rows = db.execute("SELECT * FROM study_sets ORDER BY name").fetchall()
        self.send_json(rows_to_list(rows))

    def api_create_study_set(self, db, body):
        db.execute(
            "INSERT INTO study_sets (name, description, work_duration_seconds, break_duration_seconds, long_break_seconds, sessions_before_long_break) VALUES (?, ?, ?, ?, ?, ?)",
            (body.get("name", ""), body.get("description", ""),
             body.get("work_duration_seconds", 1500), body.get("break_duration_seconds", 300),
             body.get("long_break_seconds", 900), body.get("sessions_before_long_break", 4)))
        db.commit()
        self.send_json({"id": db.execute("SELECT last_insert_rowid()").fetchone()[0]}, 201)


if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8080))
    # Resolve DB path relative to frontend dir
    if not os.path.isabs(DB_PATH):
        DB_PATH = str(FRONTEND_DIR.parent / DB_PATH)
    print(f"Study Planner Web Server")
    print(f"  Database: {DB_PATH}")
    print(f"  Listening: http://localhost:{port}")
    http.server.HTTPServer(("0.0.0.0", port), APIHandler).serve_forever()
